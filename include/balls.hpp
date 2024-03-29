#ifndef BALLS_H
#define BALLS_H

#include "raylib.h"

#define RL_COLOR_TYPE
#define RL_RECTANGLE_TYPE
#define RL_VECTOR2_TYPE
#define RL_VECTOR3_TYPE
#define RL_VECTOR4_TYPE
#define RL_QUATERNION_TYPE
#define RL_MATRIX_TYPE

#include <unordered_map>
#include <vector>

#include "raymath.h"

template <typename T>
struct Vec2 {
    T x;
    T y;
    bool operator==(Vec2<T> const& i) const { return x == i.x && y == i.y; }
};

template <>
struct Vec2<float> {
    float x;
    float y;
    operator Vector2() const { return {(float)x, (float)y}; }
};

namespace std {
template <>
struct hash<Vec2<int>> {
    std::size_t operator()(Vec2<int> const& v) const noexcept {
        auto h1 = std::hash<int>{}(v.x);
        auto h2 = std::hash<int>{}(v.y);
        h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
        return h1;
    }
};
}  // namespace std

template <typename T>
struct Vec4 {
    T x;
    T y;
    T z;
    T w;
    bool operator==(Vec2<T> const& i) const { return x == i.x && y == i.y && z == i.z && w == i.w; }
    operator Vector4() const { return {(float)x, (float)y, (float)z, (float)w}; }
};

class Ball {
   public:
    int id;
    int radius;
    float mass;
    Color color;
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    bool shouldUpdate;

    Ball(int id, int radius, float mass, Color color, Vector2 pos, Vector2 vel, Vector2 acc);

    void update(void);
    void forceUpdate(void);
    void toggleUpdate(void);

    void draw(void);

    // returns the coordinates of the top, right, bottom, and left sides of the ball in order
    Vec4<Vec2<float>> getBounds(void) const;

    bool isCollidingWith(Ball& other) const;
};

enum BallSelectionType {
    Drag,
    Shoot
};

// World class that checks for collisions using spatial hashing
class CollidingWorld {
   private:
    std::unordered_map<Vec2<int>, std::vector<Ball*>> cells;
    std::vector<Ball> balls;
    int selectedBall;
    BallSelectionType selectionType;
    int cellSize;
    Vec2<int> worldConstraint;
    bool shouldUpdate;
    int lastId;
    Ball* shooter;

    Vec2<int> hash(Vector2 position) const;

   public:
    CollidingWorld(int cellSize, Vec2<int> worldConstraint);

    void addBall(Ball ball);
    void removeBall(int id);

    bool checkBallCollision(Vec2<int> cell, int id1, int id2);
    void resolveCollisions(Vec2<int> cell);

    bool isValidCell(Vec2<int> cell);
    void buildCells(void);

    Ball* getSelected(void);
    void setSelected(Vector2 mousePos, BallSelectionType type);
    void unsetSelected(void);
    BallSelectionType getSelectionType(void) const;

    void update(Vector2 mouseCoords, bool checkCollision);
    void update(Vector2 mouseCoords);
    void toggleUpdate(void);
    bool isUpdating(void) const;

    std::vector<Vec2<int>> getRelatedCoords(Vec2<int> pos);

    int getLastBallId(void) const;
    int getBallCount(void) const;

    void draw(void);
};

#endif  // BALLS_H