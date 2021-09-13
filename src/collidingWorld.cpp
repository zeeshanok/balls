#include <math.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>

#include "balls.hpp"

CollidingWorld::CollidingWorld(int c, Vec2<int> constr) : cellSize(c), selectedBall(-1), shouldUpdate(true), lastId(-1) {
    worldConstraint = constr;
    auto [wx, wy] = worldConstraint;
    for (int i = 0; i < wx / cellSize; i++) {
        for (int j = 0; j < wy / cellSize; j++) {
            auto pos = Vec2<int>{i, j};
            cells[pos] = {};
        }
    }
}

int CollidingWorld::getLastBallId(void) const {
    return lastId;
}

std::vector<Vec2<int>> CollidingWorld::getRelatedCoords(Vec2<int> pos) {
    std::vector<Vec2<int>> possible = {pos,
                                       {pos.x - 1, pos.y - 1},
                                       {pos.x, pos.y - 1},
                                       {pos.x + 1, pos.y - 1},
                                       {pos.x + 1, pos.y},
                                       {pos.x + 1, pos.y + 1},
                                       {pos.x, pos.y + 1},
                                       {pos.x - 1, pos.y + 1},
                                       {pos.x - 1, pos.y}};
    std::vector<Vec2<int>> result = {};
    for (auto &coord : possible) {
        if (isValidCell(coord)) result.push_back(coord);
    }
    return possible;
}

Vec2<int> CollidingWorld::hash(Vector2 p) const { return {(int)p.x / cellSize, (int)p.y / cellSize}; }

bool CollidingWorld::isValidCell(Vec2<int> cell) {
    auto [wx, wy] = worldConstraint;
    auto [cx, cy] = cell;
    auto width = wx / cellSize;
    auto height = wy / cellSize;
    return cx <= width && cx >= 0 && cy <= height && cy >= 0;
}

void CollidingWorld::buildCells(void) {
    for (auto &[_, vec] : this->cells) {
        vec.clear();
    }

    for (size_t i = 0; i < balls.size(); i++) {
        auto hash_pos = hash(balls[i].pos);
        auto [a, b, c, d] = balls[i].getBounds();
        std::unordered_set<Vec2<int>> coords = {hash_pos, hash(a), hash(b), hash(c), hash(d)};
        bool added = false;

        // we want to add the ball to every cell that it touches
        for (auto &coord : coords) {
            if (isValidCell(hash_pos)) {
                cells[hash_pos].push_back(&this->balls[i]);
                added = true;
            }
        }
        if (!added) {
            std::cerr << "could not find cell for " << hash_pos.x << "," << hash_pos.y << "\n";
        }
    }
}

bool CollidingWorld::checkBallCollision(Vec2<int> cell_pos, int id1, int id2) {
    if (id1 == id2) throw std::invalid_argument("ball ids cannot be the same");
    if (isValidCell(cell_pos)) {
        std::unordered_set<int> ids = {id1, id2};
        auto cell = cells[cell_pos];
        for (auto &x : cell) {
            for (auto &y : cell) {
                std::unordered_set<int> set = {x->id, y->id};
                // dont collide with the same ball
                if (x->id != y->id && set == ids &&
                    Vector2Distance(x->pos, y->pos) <= x->radius + y->radius) {
                    return true;
                }
            }
        }
    }
    return false;
}

void CollidingWorld::resolveCollisions(Vec2<int> pos) {
    if (isValidCell(pos)) {
        auto coords = getRelatedCoords(pos);
        std::unordered_set<Ball *> c = {};
        for (auto &coord : coords) {
            auto cell = cells[coord];
            for (auto ball : cell) {
                c.insert(ball);
            }
        }
        for (auto &x : c) {
            for (auto &y : c) {
                if (x->id != y->id && x->isCollidingWith(*y)) {
                    std::cout << x->id << "," << y->id << " " << pos.x << "," << pos.y << "\n";
                    auto r1 = x->radius, r2 = y->radius;
                    auto p1 = x->pos, p2 = y->pos;
                    auto difference = Vector2Subtract(p1, p2);
                    auto rcap = Vector2Normalize(difference);
                    auto rIntersect = r1 + r2 - Vector2Distance(difference, Vector2Zero());
                    auto f1 = Vector2Add(Vector2Scale(rcap, rIntersect * 0.5), p1);
                    auto f2 = Vector2Add(Vector2Scale(Vector2Negate(rcap), rIntersect * 0.5), p2);
                    x->pos = f1;
                    y->pos = f2;
                }
            }
        }
    }
}

void CollidingWorld::addBall(Ball ball) {
    this->balls.push_back(ball);
    lastId = ball.id;

    buildCells();
}

void CollidingWorld::removeBall(int id) {
    balls.erase(std::remove_if(balls.begin(), balls.end(), [id](Ball const &b) { return b.id == id; }));
    lastId = balls.empty() ? -1 : balls.back().id;

    buildCells();
}

void CollidingWorld::setSelected(Vector2 mousePos) {
    if (selectedBall == -1) {
        for (auto &ball : balls) {
            if (Vector2Distance(mousePos, ball.pos) <= ball.radius) {
                selectedBall = ball.id;
            }
        }
    }
}

void CollidingWorld::unsetSelected(void) { selectedBall = -1; }

void CollidingWorld::update(bool checkCollision) {
    buildCells();

    std::unordered_set<int> updated = {};

    for (auto &[pos, cell] : this->cells) {
        for (auto &x : cell) {
            if (x->id == selectedBall) {
                x->pos = GetMousePosition();
            } else if (shouldUpdate && updated.find(x->id) == updated.end()) {
                // update only if ball hasnt been updated in another cell before
                x->update();

                Vec2<bool> outbound = {x->pos.x >= worldConstraint.x - x->radius,
                                       x->pos.y >= worldConstraint.y - x->radius};
                Vec2<bool> inbound = {x->pos.x - x->radius < 0, x->pos.y - x->radius < 0};

                const auto xfunc = [x]() {
                    x->vel.x = -x->vel.x;
                    x->acc.x = -x->acc.x;
                };
                const auto yfunc = [x]() {
                    x->vel.y = -x->vel.y;
                    x->acc.y = -x->acc.y;
                };

                if (outbound.x) {
                    x->pos.x = worldConstraint.x - x->radius;
                    xfunc();
                } else if (inbound.x) {
                    x->pos.x = x->radius;
                    xfunc();
                }

                if (outbound.y) {
                    x->pos.y = worldConstraint.y - x->radius;
                    yfunc();
                } else if (inbound.y) {
                    x->pos.y = x->radius;
                    yfunc();
                }
                updated.insert(x->id);
            }
        }
        if (checkCollision) resolveCollisions(pos);
    }
}

void CollidingWorld::update(void) { update(false); }

void CollidingWorld::toggleUpdate(void) { this->shouldUpdate = !this->shouldUpdate; }

bool CollidingWorld::isUpdating(void) const { return this->shouldUpdate; }

void CollidingWorld::draw(void) {
    for (int i = 0; i < worldConstraint.x / cellSize; i++) {
        DrawLine(i * cellSize, 0, i * cellSize, worldConstraint.y, GRAY);
        for (int j = 0; j < worldConstraint.y / cellSize; j++) {
            DrawLine(0, j * cellSize, worldConstraint.x, j * cellSize, GRAY);
        }
    }
    for (auto &[cell, balls] : cells) {
        for (auto &x : balls) {
            x->draw();
            DrawText((std::to_string(cell.x) + "," + std::to_string(cell.y)).c_str(), x->pos.x, x->pos.y, 14,
                     WHITE);
        }
    }
}