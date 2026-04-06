#include "raylib.h"
#include "Engine.h"

using namespace Engine;

const int FPS = 60;
const int TPS = 30;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const auto WINDOW_TITLE = "Test";

int main()
{
    World world;

    return App(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, FPS, TPS,
        //load
        [&]()
        {
            Object* testObj = world.registry.CreateObject();
            auto* testPos = world.registry.CreateComponent<Position2D>(testObj, 100.0f, 100.0f, 0.0f);
            auto* testCircle = world.registry.CreateComponent<Draw2DCircle>(testObj, 25.0f, 0.0f, 0.0f, RED, 1);
        },
        //update
        [&](float dt)
        {

        },
        //draw
        [&]()
        {
            ClearBackground(RAYWHITE);

            for (auto* draw : Draw2D::GetList())
            {
                Object* obj = draw->GetOwner();
                auto* pos = obj->GetComponent<Position2D>();
                if (!pos) continue;
                draw->Draw(pos->x, pos->y, pos->a);
            }

            DrawFPS(10, 10);
            DrawTPS(10, 30);
        }
        );
}
