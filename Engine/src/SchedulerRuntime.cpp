#include "SchedulerRuntime.h"

#include <vector>
#include <cstdio>

#include "LuaRuntime.h"

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
}

namespace Engine
{
    namespace
    {
        struct Task
        {
            int id = 0;
            float timeLeft = 0.0f;
            float interval = 0.0f;
            bool repeating = false;
            bool cancelled = false;
            int luaRef = LUA_NOREF;
        };

        static std::vector<Task> g_tasks;
        static int g_nextId = 1;
        static bool g_active = false;
    }

    void SchedulerRuntime::Init()
    {
        g_tasks.clear();
        g_nextId = 1;
        g_active = true;
    }

    void SchedulerRuntime::Stop()
    {
        lua_State* L = LuaRuntime::GetState();

        if (L)
        {
            for (auto& task : g_tasks)
            {
                if (task.luaRef != LUA_NOREF)
                    luaL_unref(L, LUA_REGISTRYINDEX, task.luaRef);
            }
        }

        g_tasks.clear();
        g_nextId = 1;
        g_active = false;
    }

    int SchedulerRuntime::After(float seconds, int luaRef)
    {
        Task task;
        task.id = g_nextId++;
        task.timeLeft = seconds;
        task.interval = seconds;
        task.repeating = false;
        task.cancelled = false;
        task.luaRef = luaRef;
        g_tasks.push_back(task);
        return task.id;
    }

    int SchedulerRuntime::Every(float seconds, int luaRef)
    {
        Task task;
        task.id = g_nextId++;
        task.timeLeft = seconds;
        task.interval = seconds;
        task.repeating = true;
        task.cancelled = false;
        task.luaRef = luaRef;
        g_tasks.push_back(task);
        return task.id;
    }

    void SchedulerRuntime::Cancel(int id)
    {
        for (auto& task : g_tasks)
        {
            if (task.id == id)
            {
                task.cancelled = true;
                return;
            }
        }
    }

    void SchedulerRuntime::Update(float dt)
    {
        if (!g_active) return;

        lua_State* L = LuaRuntime::GetState();
        if (!L) return;

        const LuaExecutionContext previous = LuaRuntime::GetCurrentContext();
        LuaRuntime::SetCurrentContext(LuaExecutionContext::Update);

        for (auto& task : g_tasks)
        {
            if (task.cancelled) continue;

            task.timeLeft -= dt;
            if (task.timeLeft > 0.0f) continue;

            lua_rawgeti(L, LUA_REGISTRYINDEX, task.luaRef);

            if (lua_pcall(L, 0, 0, 0) != LUA_OK)
            {
                const char* err = lua_tostring(L, -1);
                if (err) std::printf("Lua scheduler error: %s\n", err);
                lua_pop(L, 1);
            }

            if (task.repeating)
                task.timeLeft += task.interval;
            else
                task.cancelled = true;
        }

        LuaRuntime::SetCurrentContext(previous);

        for (size_t i = 0; i < g_tasks.size();)
        {
            if (g_tasks[i].cancelled)
            {
                if (g_tasks[i].luaRef != LUA_NOREF)
                    luaL_unref(L, LUA_REGISTRYINDEX, g_tasks[i].luaRef);

                g_tasks.erase(g_tasks.begin() + static_cast<long long>(i));
            }
            else
            {
                ++i;
            }
        }
    }
}