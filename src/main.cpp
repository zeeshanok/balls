#include <time.h>

#include <iostream>
#include <string>

#include "balls.hpp"

int main(void) {
    srand(time(NULL));
    const int screenHeight = 900, screenWidth = 900;
    const int balls = 50;
    CollidingWorld world(50, Vec2<int>{screenWidth, screenHeight});

    Color colors[] = {BLUE, RED, GREEN, YELLOW};

    const auto randBall = [colors](int id) {
        Vector2 pos = {rand() % screenWidth, rand() % screenHeight};
        Vector2 vel = {10 + (rand() % 40), 10 + (rand() % 40)};
        // auto vel = Vector2Zero();
        auto acc = Vector2Zero();
        auto color = colors[rand() % 4];
        // auto radius = 40 + (rand() % 20);
        auto radius = 10;
        return Ball(id, radius, radius, color, pos, vel, acc);
    };

    for (size_t i = 0; i < balls; i++) {
        world.addBall(randBall(i));
    }

    InitWindow(screenWidth, screenHeight, "balls");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            world.setSelected(GetMousePosition());
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            world.unsetSelected();
        }
        if (IsKeyPressed(KEY_SPACE)) {
            world.toggleUpdate();
        }
        if (IsKeyPressed(KEY_A)) {
            world.addBall(randBall(world.getLastBallId() + 1));
        }
        if (IsKeyPressed(KEY_D)) {
            world.removeBall(world.getLastBallId());
        }

        world.update(true);

        BeginDrawing();

        ClearBackground(BLACK);

        world.draw();
        DrawText((std::to_string(GetFPS()) + (world.isUpdating() ? "" : "  Paused") +
                  "\nBalls: " + std::to_string(world.getBallCount()))
                     .c_str(),
                 0, 0, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}