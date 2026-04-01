#include "Engine.h"

namespace Engine
{
    int App(int w, int h, const char* title, int fps, int tps,
        const std::function<void()>& load,
        const std::function<void(float)>& update,
        const std::function<void()>& draw)
    {
        InitWindow(w, h, title);
        InitRuntime();
        SetTargetFPS(fps);
        SetTargetTPS(tps);

        load();

        while (!WindowShouldClose())
        {
            Update(GetFrameTime(), update);

            Draw(draw);
        }

        CloseWindow();
        return 0;
    }
}
