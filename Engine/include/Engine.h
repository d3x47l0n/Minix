#pragma once

#include <memory>
#include <functional>

#include "raylib.h"
#include "box2d/box2d.h"

#include "MinixRuntime.h"
#include "LuaRuntime.h"
#include "PhysicsRuntime.h"
#include "InputRuntime.h"
#include "SchedulerRuntime.h"

#include "Registry.h"
#include "Camera.h"
#include "Messager.h"

#include "ComponentDraw.h"
#include "ComponentPosition.h"
#include "ComponentPhysics.h"

#include "TypeObject.h"
#include "TypeComponent.h"
#include "TypeCoords.h"
#include "TypeAction.h"

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
    inline void InitMinixRuntime()
    {
        Engine::MinixRuntime::Init();
    }

    inline void MinixUpdate(float dt, const std::function<void(float)>& func)
    {
        Engine::MinixRuntime::Update(dt, func);
    }

    inline void SetTargetTPS(int TPS)
    {
        Engine::MinixRuntime::SetTargetTPS(TPS);
    }

    inline void DrawTPS(int x, int y)
    {
        Engine::MinixRuntime::DrawTPS(x, y);
    }

    inline void InitLuaRuntime()
    {
        Engine::LuaRuntime::Init();
    }

    inline void StopLuaRuntime()
    {
        Engine::LuaRuntime::Stop();
    }

    inline void LuaLoad()
    {
        Engine::LuaRuntime::CallLoad();
    }

    inline void LuaUpdate(float dt)
    {
        Engine::LuaRuntime::CallUpdate(dt);
    }

    inline void LuaDraw()
    {
        Engine::LuaRuntime::CallDraw();
    }

    inline void InitInputRuntime()
    {
        Engine::InputRuntime::Init();
    }

    inline void StopInputRuntime()
    {
        Engine::InputRuntime::Stop();
    }

    inline void LockInputRuntime()
    {
        Engine::InputRuntime::Lock();
    }

    inline void UpdateInput(float dt)
    {
        Engine::InputRuntime::Update(dt);
    }

    inline void InitPhysicsRuntime()
    {
        Engine::PhysicsRuntime::Init();
    }

    inline void StopPhysicsRuntime()
    {
        Engine::PhysicsRuntime::Stop();
    }

    inline void UpdatePhysics(float dt)
    {
        Engine::PhysicsRuntime::Update(dt);
    }

    inline void InitCamera(int w, int h)
    {
        Engine::Camera::Init(w, h);
    }

    inline void StopCamera()
    {
        Engine::Camera::Stop();
    }

    inline void UpdateCamera(float dt)
    {
        Engine::Camera::Update(dt);
    }

    inline void InitSchedulerRuntime()
    {
        Engine::SchedulerRuntime::Init();
    }

    inline void StopSchedulerRuntime()
    {
        Engine::SchedulerRuntime::Stop();
    }

    inline void UpdateScheduler(float dt)
    {
        Engine::SchedulerRuntime::Update(dt);
    }

    int App(int w, int h, const char* title, int fps, int tps,
        const std::function<void()>& load,
        const std::function<void(float)>& update,
        const std::function<void()>& draw);
}