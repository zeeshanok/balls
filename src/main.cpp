#include <time.h>

#include <iostream>
#include <string>

#include "balls.hpp"

int main(void) {
    srand(time(NULL));
    const int screenHeight = 900, screenWidth = 900;
    const int balls =10;
    CollidingWorld world(50, Vec2<int>{screenWidth, screenHeight});

    Color colors[] = {BLUE, RED, GREEN, YELLOW};

    const auto randBall = [colors](int id) {
        Vector2 pos = {rand() % screenWidth, rand() % screenHeight};
        // Vector2 vel = {10 + (rand() % 40), 10 + (rand() % 40)};
        auto vel = Vector2Zero();
        auto acc = Vector2Zero();
        auto color = colors[rand() % 4];
        // auto radius = 40 + (rand() % 20);
        auto radius = 30;
        return Ball(id, radius, radius, color, pos, vel, acc);
    };

    for (size_t i = 0; i < balls; i++) {
        world.addBall(randBall(i));
    }

    InitWindow(screenWidth, screenHeight, "balls");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        bool rPressed = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            world.setSelected(GetMousePosition(), BallSelectionType::Drag);
        }
        if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
            world.setSelected(GetMousePosition(), BallSelectionType::Shoot);
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
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

        world.update(GetMousePosition(), true);

        BeginDrawing();

        ClearBackground(BLACK);

        world.draw();
        DrawText((std::to_string(GetFPS()) + (world.isUpdating() ? "" : "  Paused") +
                  "\nBalls: " + std::to_string(world.getBallCount()))
                     .c_str(),
                 0, 0, 20, WHITE);

        auto selected = world.getSelected();
        auto type = world.getSelectionType();
        if (selected != nullptr && type == BallSelectionType::Shoot) {
            auto mouse = GetMousePosition();
            DrawLine(mouse.x, mouse.y, selected->pos.x, selected->pos.y, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}