#include <time.h>

#include <iostream>
#include <string>

#include "balls.hpp"

int main(void) {
    srand(time(NULL));
    const int screenHeight = 900, screenWidth = 900;
    CollidingWorld world(90, Vec2<int>{screenWidth, screenHeight});

    Color colors[] = {BLUE, RED, GREEN, YELLOW};

    for (size_t i = 0; i < 20; i++) {
        Vector2 pos = {rand() % screenWidth, rand() % screenHeight};
        Vector2 vel = {200 + (rand() % 40), 200 + (rand() % 40)};
        auto acc = Vec2<float> {1, 0};
        auto color = colors[rand() % 4];
        auto radius = 20 + (rand() % 40);
        world.addBall(Ball(i, radius, radius, color, pos, vel, acc));
    }

    InitWindow(screenWidth, screenHeight, "balls");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // for (auto &ball : balls) {
        //     ball.update();
        //     if (ball.pos.x >= screenWidth || ball.pos.x < 0) {
        //         ball.vel.x = -ball.vel.x;
        //         ball.acc.x = -ball.acc.x;
        //     }
        //     if (ball.pos.y >= screenHeight || ball.pos.y < 0) {
        //         ball.vel.y = -ball.vel.y;
        //         ball.acc.y = -ball.acc.y;
        //     }
        // }

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            world.setSelected(GetMousePosition());
        }
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            world.unsetSelected();
        }
        if (IsKeyPressed(KEY_SPACE)) {
            world.toggleUpdate();
        }

        world.update(true);

        BeginDrawing();

        ClearBackground(BLACK);

        world.draw();
        DrawText((std::to_string(GetFPS()) + (world.isUpdating() ? "" : "  Paused")).c_str(), 0, 0, 20,
                 WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}