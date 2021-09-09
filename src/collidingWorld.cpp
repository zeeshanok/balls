#include <math.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>

#include "balls.hpp"

CollidingWorld::CollidingWorld(int c, Vec2<int> constr) : cellSize(c), selectedBall(-1), shouldUpdate(true) {
    worldConstraint = constr;
    auto [wx, wy] = worldConstraint;
    for (int i = 0; i < wx / cellSize; i++) {
        for (int j = 0; j < wy / cellSize; j++) {
            auto pos = Vec2<int>{i, j};
            cells[pos] = {};
        }
    }
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

void CollidingWorld::resolveCollisions(void) {
    for (auto &[_, cell] : cells) {
        for (auto &x : cell) {
            for (auto &y : cell) {
                if (x->id != y->id && x->isCollidingWith(*y)) {
                    // std::cout << "collision between ball" << x->id << " and " << y->id << "\n";
                }
            }
        }
    }
}

void CollidingWorld::addBall(Ball ball) {
    this->balls.push_back(ball);

    buildCells();
}

void CollidingWorld::removeBall(int id) {
    balls.erase(std::remove_if(balls.begin(), balls.end(), [id](Ball const &b) { return b.id == id; }));

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

    for (auto &[_, vec] : this->cells) {
        for (auto &x : vec) {
            if (x->id == selectedBall) {
                x->pos = GetMousePosition();
            } else if (shouldUpdate) {
                // update only if ball hasnt been updated in another cell before
                if (updated.find(x->id) == updated.end()) {
                    x->update();
                    if (x->pos.x >= worldConstraint.x - x->radius || x->pos.x - x->radius < 0) {
                        x->vel.x = -x->vel.x;
                        x->acc.x = -x->acc.x;
                    }
                    if (x->pos.y >= worldConstraint.y - x->radius || x->pos.y - x->radius < 0) {
                        x->vel.y = -x->vel.y;
                        x->acc.y = -x->acc.y;
                    }
                }
                if (checkCollision) resolveCollisions();
                updated.insert(x->id);
            }
        }
    }
}

void CollidingWorld::update(void) { update(false); }

void CollidingWorld::toggleUpdate(void) { this->shouldUpdate = !this->shouldUpdate; }

bool CollidingWorld::isUpdating(void) const { return this->shouldUpdate; }

void CollidingWorld::draw(void) {
    for (auto &[cell, balls] : cells) {
        for (auto &x : balls) {
            x->draw();
            DrawText(std::to_string(x->id).c_str(), x->pos.x, x->pos.y, 14, WHITE);
        }
    }
}