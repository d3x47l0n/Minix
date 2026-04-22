#include "LuaRuntime.h"
#include "LuaAPI.h"

#include <cstdio>
#include <string>

namespace Engine
{
    lua_State* LuaRuntime::_L = nullptr;
    LuaExecutionContext LuaRuntime::_context = LuaExecutionContext::Outside;

    int LuaRuntime::Lua_Require(lua_State* L)
    {
        const char* name = luaL_checkstring(L, 1);

        std::string path = "scripts/";
        path += name;
        path += ".lua";

        if (luaL_loadfile(L, path.c_str()) != LUA_OK)
        {
            return lua_error(L);
        }

        if (lua_pcall(L, 0, 1, 0) != LUA_OK)
        {
            return lua_error(L);
        }

        return 1;
    }

    void LuaRuntime::OpenSafeLibraries(lua_State* L)
    {
        // Base library
        luaL_requiref(L, "_G", luaopen_base, 1);
        lua_pop(L, 1);

        // Safe standard libs
        luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
        lua_pop(L, 1);

        luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
        lua_pop(L, 1);

        luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
        lua_pop(L, 1);

        luaL_requiref(L, LUA_UTF8LIBNAME, luaopen_utf8, 1);
        lua_pop(L, 1);

        // Optional but usually harmless/useful
        luaL_requiref(L, LUA_COLIBNAME, luaopen_coroutine, 1);
        lua_pop(L, 1);
    }

    void LuaRuntime::RemoveDangerousGlobals(lua_State* L)
    {
        // Remove dangerous globals from base lib
        lua_pushnil(L); lua_setglobal(L, "dofile");
        lua_pushnil(L); lua_setglobal(L, "loadfile");
        lua_pushnil(L); lua_setglobal(L, "load");

        // Remove dangerous standard libs entirely
        lua_pushnil(L); lua_setglobal(L, "io");
        lua_pushnil(L); lua_setglobal(L, "os");
        lua_pushnil(L); lua_setglobal(L, "package");
        lua_pushnil(L); lua_setglobal(L, "debug");

        // Also remove require explicitly in case something reintroduced package-ish access
        lua_pushnil(L); lua_setglobal(L, "require");
    }

    bool LuaRuntime::Init()
    {
        _L = luaL_newstate();
        if (!_L) return false;

        OpenSafeLibraries(_L);
        RemoveDangerousGlobals(_L);

        lua_pushcfunction(_L, Lua_Require);
        lua_setglobal(_L, "require");

        LuaAPI::Init(_L);

        return true;
    }

    void LuaRuntime::Stop()
    {
        if (_L)
        {
            lua_close(_L);
            _L = nullptr;
        }

        _context = LuaExecutionContext::Outside;
    }

    bool LuaRuntime::LoadFile(const char* path)
    {
        if (!_L) return false;

        if (luaL_loadfile(_L, path) != LUA_OK)
        {
            const char* err = lua_tostring(_L, -1);
            if (err) std::printf("Lua load error: %s\n", err);
            lua_pop(_L, 1);
            return false;
        }

        if (lua_pcall(_L, 0, 0, 0) != LUA_OK)
        {
            const char* err = lua_tostring(_L, -1);
            if (err) std::printf("Lua error: %s\n", err);
            lua_pop(_L, 1);
            return false;
        }

        LuaAPI::Refresh(_L);
        return true;
    }

    bool LuaRuntime::PushMinixFunction(const char* name)
    {
        if (!_L) return false;

        lua_getglobal(_L, "minix");
        if (!lua_istable(_L, -1))
        {
            lua_pop(_L, 1);
            return false;
        }

        lua_getfield(_L, -1, name);
        if (!lua_isfunction(_L, -1))
        {
            lua_pop(_L, 2);
            return false;
        }

        return true;
    }

    bool LuaRuntime::CallMinixVoid(const char* name, LuaExecutionContext context)
    {
        if (!PushMinixFunction(name)) return false;

        LuaExecutionContext oldContext = _context;
        _context = context;

        if (lua_pcall(_L, 0, 0, 0) != LUA_OK)
        {
            const char* err = lua_tostring(_L, -1);
            if (err) std::printf("Lua error in minix.%s: %s\n", name, err);
            lua_pop(_L, 2);
            _context = oldContext;
            return false;
        }

        lua_pop(_L, 1);
        _context = oldContext;
        return true;
    }

    bool LuaRuntime::CallMinixNumber(const char* name, float value, LuaExecutionContext context)
    {
        if (!PushMinixFunction(name)) return false;

        lua_pushnumber(_L, value);

        LuaExecutionContext oldContext = _context;
        _context = context;

        if (lua_pcall(_L, 1, 0, 0) != LUA_OK)
        {
            const char* err = lua_tostring(_L, -1);
            if (err) std::printf("Lua error in minix.%s: %s\n", name, err);
            lua_pop(_L, 2);
            _context = oldContext;
            return false;
        }

        lua_pop(_L, 1);
        _context = oldContext;
        return true;
    }

    void LuaRuntime::CallLoad()
    {
        CallMinixVoid("load", LuaExecutionContext::Load);
    }

    void LuaRuntime::CallUpdate(float dt)
    {
        CallMinixNumber("update", dt, LuaExecutionContext::Update);
    }

    void LuaRuntime::CallDraw()
    {
        CallMinixVoid("draw", LuaExecutionContext::Draw);
    }

    lua_State* LuaRuntime::GetState()
    {
        return _L;
    }

    void LuaRuntime::SetCurrentContext(LuaExecutionContext context)
    {
        _context = context;
    }

    LuaExecutionContext LuaRuntime::GetCurrentContext()
    {
        return _context;
    }

    const char* LuaRuntime::GetCurrentContextName()
    {
        switch (_context)
        {
        case LuaExecutionContext::Load:   return "minix.load";
        case LuaExecutionContext::Update: return "minix.update";
        case LuaExecutionContext::Draw:   return "minix.draw";
        default:                         return "outside";
        }
    }
}