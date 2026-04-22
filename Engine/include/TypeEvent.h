#pragma once

#include <vector>
#include <functional>

struct lua_State;

namespace Engine
{
    class Component;
    class Object;

    class TypeEvent
    {
    public:
        explicit TypeEvent(const char* name = nullptr)
            : _name(name)
        {
        }

        virtual ~TypeEvent() = default;

        void SetOwner(Component* owner)
        {
            _owner = owner;
        }

        Component* GetOwner() const
        {
            return _owner;
        }

        const char* GetName() const
        {
            return _name;
        }

        std::vector<int>& GetLuaRefs()
        {
            return _luaRefs;
        }

        const std::vector<int>& GetLuaRefs() const
        {
            return _luaRefs;
        }

        void AddLuaRef(int ref)
        {
            _luaRefs.push_back(ref);
        }

        void ClearLuaRefs()
        {
            _luaRefs.clear();
        }

        virtual int PushPayload(lua_State* L, const void* payload) = 0;

    private:
        Component* _owner = nullptr;
        const char* _name = nullptr;
        std::vector<int> _luaRefs;
    };
}