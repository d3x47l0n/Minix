#include "Engine.h"

namespace Engine
{
    int App(int w, int h, const char* title, int fps, int tps,
        const std::function<void()>& load,
        const std::function<void(float)>& update,
        const std::function<void()>& draw)
    {
        InitWindow(w, h, title);
        InitAudioDevice();

        InitMinixRuntime();
        InitPhysicsRuntime();
        InitLuaRuntime();
        InitSchedulerRuntime();
        InitInputRuntime();
        InitCamera(w, h);

        SetTargetFPS(fps);
        SetTargetTPS(tps);

        load();

        LockInputRuntime();

        while (!WindowShouldClose())
        {
            MinixUpdate(GetFrameTime(),
                [&](float dt)
                {
                    UpdateInput(dt);
                    UpdateScheduler(dt);
                    UpdatePhysics(dt);
                    UpdateCamera(dt);
                    update(dt);
                }
            );

            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(Camera::GetRaylibCamera());

            for (auto* drawComponent : Draw2D::GetSortedList())
            {
                Object* obj = drawComponent->GetOwner();
                if (!obj) continue;

                auto* pos = obj->GetComponent<Position2D>();
                if (!pos) continue;

                drawComponent->Draw(*pos, Coords::GetScale());
            }

            EndMode2D();

            draw();

            DrawFPS(10, 10);
            DrawTPS(10, 30);
            DrawText("MINIX DEMO", w / 2 - MeasureText("MINIX DEMO",20) / 2, 10, 20, BLUE);

            EndDrawing();
        }

        StopCamera();
        StopSchedulerRuntime();
        StopInputRuntime();
        StopPhysicsRuntime();
        StopLuaRuntime();
        CloseAudioDevice();
        CloseWindow();
        return 0;
    }
}