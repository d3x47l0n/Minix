#pragma once

namespace Engine
{
    class SchedulerRuntime
    {
    public:
        static void Init();
        static void Stop();
        static void Update(float dt);

        static int After(float seconds, int luaRef);
        static int Every(float seconds, int luaRef);
        static void Cancel(int id);
    };
}