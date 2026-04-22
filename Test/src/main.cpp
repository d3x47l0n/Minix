#include "Engine.h"

using namespace Engine;

const int FPS = 60;
const int TPS = 30;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const auto WINDOW_TITLE = "Test";

const float SCALE = 50.0f;

int main()
{
    Coords::SetScale(SCALE);

    return App(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, FPS, TPS,
        //load
        [&]()
        {
            LuaRuntime::LoadFile("main.lua");
            LuaLoad();

            for (auto* obj : Registry::GetAllObjects())
            {
                auto* pos = obj->GetComponent<Position2D>();
                auto* phys = obj->GetComponent<Physics2D>();

                if (!pos || !phys) continue;

                phys->SetXYA(pos->GetXYA());
            }
        },
        //update
        [&](float dt)
        {
            LuaUpdate(dt);
        },
        //draw
        [&]()
        {
            LuaDraw();
        }
    );
}