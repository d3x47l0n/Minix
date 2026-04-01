#pragma once

#include <memory>
#include <functional>

#include "raylib.h"
#include "Runtime.h"
#include "World.h"

/*
Minix C++ Naming Conventions

Classes / Structs:
    PascalCase
    Example: Runtime, World

Functions:
    PascalCase
    Example: InitRuntime(), DrawTPS()

Namespaces:
    PascalCase
    Example: Engine

Local variables / parameters:
    camelCase
    Example: deltaTime, catchup

Private class members:
    _camelCase
    Example: _world, _accTick

Constants:
    ALL_CAPS
    Example: MAX_ENTITIES
*/

namespace Engine
{

    template<typename F>
    inline void Draw(F func)
    {
        BeginDrawing();
        func();
        EndDrawing();
    }

    inline void InitRuntime()
    {
        Engine::Runtime::Init();
    }

    inline void Update(float dt, const std::function<void(float)>& func)
    {
        Engine::Runtime::Update(dt, func);
    }

    inline void SetTargetTPS(int TPS)
    {
        Engine::Runtime::SetTargetTPS(TPS);
    }

    inline void DrawTPS(int x, int y)
    {
        Engine::Runtime::DrawTPS(x, y);
    }

    // da da da stiu ca std::function adauga vreo 5-15 apeluri in plus fata de template dar asta e doar
    // un wrapper ca sa nu trebuiasca sa pun codul direct in header ca in epoca de piatra
    int App(int w, int h, const char* title, int fps, int tps,
        const std::function<void()>& load,
        const std::function<void(float)>& update,
        const std::function<void()>& draw);
}
