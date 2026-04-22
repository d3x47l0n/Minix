#include "Messager.h"

#include <cstdio>

#include "TypeEvent.h"
#include "LuaRuntime.h"
#include "LuaAPI.h"

namespace Engine
{
    void Messager::BindLua(TypeEvent& eventRef, lua_State* L, int funcIndex)
    {
        if (!lua_isfunction(L, funcIndex))
            return;

        lua_pushvalue(L, funcIndex);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        eventRef.AddLuaRef(ref);
    }

    void Messager::Clear(TypeEvent& eventRef, lua_State* L)
    {
        if (!L)
        {
            eventRef.ClearLuaRefs();
            return;
        }

        for (int ref : eventRef.GetLuaRefs())
            luaL_unref(L, LUA_REGISTRYINDEX, ref);

        eventRef.ClearLuaRefs();
    }

    void Messager::CallEvent(TypeEvent& eventRef, const void* payload)
    {
        lua_State* L = LuaRuntime::GetState();
        if (!L) return;

        const LuaExecutionContext previous = LuaRuntime::GetCurrentContext();
        LuaRuntime::SetCurrentContext(LuaExecutionContext::Update);

        for (int ref : eventRef.GetLuaRefs())
        {
            lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

            int nargs = eventRef.PushPayload(L, payload);

            if (lua_pcall(L, nargs, 0, 0) != LUA_OK)
            {
                const char* err = lua_tostring(L, -1);
                lua_pop(L, 1);
            }
        }

        LuaRuntime::SetCurrentContext(previous);
    }
}