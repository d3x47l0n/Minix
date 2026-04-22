#pragma once

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace Engine
{
    enum class LuaExecutionContext
    {
        Outside,
        Load,
        Update,
        Draw
    };

    class LuaRuntime
    {
    public:
        static bool Init();
        static void Stop();
        static bool LoadFile(const char* path);

        static void CallLoad();
        static void CallUpdate(float dt);
        static void CallDraw();

        static lua_State* GetState();

        static void SetCurrentContext(LuaExecutionContext context);
        static LuaExecutionContext GetCurrentContext();
        static const char* GetCurrentContextName();

    private:
        static lua_State* _L;
        static LuaExecutionContext _context;

        static bool PushMinixFunction(const char* name);

        static bool CallMinixVoid(const char* name, LuaExecutionContext context);
        static bool CallMinixNumber(const char* name, float value, LuaExecutionContext context);

        static int Lua_Require(lua_State* L);
        static void OpenSafeLibraries(lua_State* L);
        static void RemoveDangerousGlobals(lua_State* L);
    };
}