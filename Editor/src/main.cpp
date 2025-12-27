#include "raylib.h"
#include "Engine.h"

int main()
{
    InitWindow(800, 600, "Editor");
    SetTargetFPS(60);

    Engine::Hello();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Editor running", 20, 20, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
