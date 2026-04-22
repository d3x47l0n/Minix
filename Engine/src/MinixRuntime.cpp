#include "raylib.h"
#include "MinixRuntime.h"
#include <cstdio>
#include <memory>
#include <functional>

namespace Engine
{
    float MinixRuntime::_accTick = 0.0f;
    float MinixRuntime::_step = 1.0f / 30.0f;
    float MinixRuntime::_accCount = 0.0f;
    int MinixRuntime::_count = 0;
    int MinixRuntime::_TPS = 0;
    bool MinixRuntime::_active = false;

    void MinixRuntime::Init()
    {
        _accTick = 0.0f;
        _accCount = 0.0f;
        _count = 0;
        _TPS = 0;
        _active = true;
    }

    void MinixRuntime::Reset()
    {
        _accTick = 0.0f;
        _accCount = 0.0f;
        _count = 0;
        _TPS = 0;
        _active = false;
    }

    void MinixRuntime::Start()
    {
        _active = true;
    }

    void MinixRuntime::Stop()
    {
        _active = false;
    }

    void MinixRuntime::Update(float dt, const std::function<void(float)>& func)
    {
        if (!_active) return;
        float deltaTime = (dt <= 0.25f) ? dt : 0.25f;
        int catchup = 0;
        _accTick += deltaTime;
        _accCount += deltaTime;
        while (_accTick >= _step && catchup < 10)
        {
            _accTick -= _step;
            if (_accTick < 0.0f)
                _accTick = 0.0f;
            _count++;
            catchup++;

            func(_step);
        }
        if (_accCount >= 0.25f)
        {
            _accCount -= 0.25f;
            if (_accCount < 0.0f)
                _accCount = 0.0f;
            _TPS = _count * 4;
            _count = 0;
        }
    }

    int MinixRuntime::GetTPS()
    {
        return _TPS;
    }

    void MinixRuntime::SetTargetTPS(int TPS)
    {
        if (TPS <= 0) TPS = 1;
        _step = 1.0f / (float)TPS;
    }

    void MinixRuntime::DrawTPS(int x, int y)
    {
        char tpsText[32];
        snprintf(tpsText, sizeof(tpsText), "%d TPS", MinixRuntime::GetTPS());
        DrawText(tpsText, x, y, 20, ORANGE);
    }
}
