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

        },
        //update
        [&](float dt)
        {

        },
        //draw
        [&]()
        {
            ClearBackground(RAYWHITE);
            DrawFPS(10, 10);
            DrawTPS(10, 30);
        }
        );
}
