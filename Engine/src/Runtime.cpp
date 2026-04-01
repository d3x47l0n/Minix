#include "raylib.h"
#include "Runtime.h"
#include "World.h"
#include <cstdio>
#include <memory>
#include <functional>

namespace Engine
{
    float Runtime::_accTick = 0.0f;
    float Runtime::_step = 1.0f / 30.0f;
    float Runtime::_accCount = 0.0f;
    int Runtime::_count = 0;
    int Runtime::_TPS = 0;
    bool Runtime::_active = false;

    void Runtime::Init()
    {
        _accTick = 0.0f;
        _accCount = 0.0f;
        _count = 0;
        _TPS = 0;
        _active = true;
    }

    void Runtime::Reset()
    {
        _accTick = 0.0f;
        _accCount = 0.0f;
        _count = 0;
        _TPS = 0;
        _active = false;
    }

    void Runtime::Start()
    {
        _active = true;
    }

    void Runtime::Stop()
    {
        _active = false;
    }

    void Runtime::Update(float dt, const std::function<void(float)>& func)
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

    int Runtime::GetTPS()
    {
        return _TPS;
    }

    void Runtime::SetTargetTPS(int TPS)
    {
        if (TPS <= 0) TPS = 1;
        _step = 1.0f / (float)TPS;
    }

    void Runtime::DrawTPS(int x, int y)
    {
        char tpsText[32];
        snprintf(tpsText, sizeof(tpsText), "%d TPS", Runtime::GetTPS());
        DrawText(tpsText, x, y, 20, ORANGE);
    }
}