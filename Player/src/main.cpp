#include "raylib.h"

int main()
{
    InitWindow(800, 600, "Player");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Player running", 20, 20, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
