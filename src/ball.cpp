#include <iostream>

#include "balls.hpp"

Ball::Ball(int i, int r, float m, Color c, Vector2 p, Vector2 v, Vector2 a)
    : id(i), radius(r), mass(m), color(c), pos(p), vel(v), acc(a), shouldUpdate(true) {}

void Ball::update(void) {
    if (shouldUpdate) {
        this->acc = Vector2Scale(this->vel, -0.01);
        this->vel = Vector2Add(this->vel, this->acc);
        this->pos = Vector2Add(this->pos, Vector2Scale(this->vel, GetFrameTime()));
    }
}

void Ball::draw(void) { DrawCircle(pos.x, pos.y, radius, color); }

void Ball::toggleUpdate(void) { this->shouldUpdate = !shouldUpdate; }

bool Ball::isCollidingWith(Ball &other) const {
    return Vector2Distance(other.pos, pos) <= (other.radius + radius);
}

Vec4<Vec2<float>> Ball::getBounds(void) const {
    return {
        {pos.x, pos.y - radius}, {pos.x + radius, pos.y}, {pos.x, pos.y + radius}, {pos.x - radius, pos.y}};
}