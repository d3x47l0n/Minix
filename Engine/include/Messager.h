#pragma once

struct lua_State;

namespace Engine
{
    class TypeEvent;

    class Messager
    {
    public:
        static void BindLua(TypeEvent& eventRef, lua_State* L, int funcIndex);
        static void Clear(TypeEvent& eventRef, lua_State* L);
        static void CallEvent(TypeEvent& eventRef, const void* payload = nullptr);
    };
}