#include "LuaAPI.h"

#include <vector>
#include <cstring>
#include <cstdio>

#include "Registry.h"
#include "PhysicsRuntime.h"
#include "LuaRuntime.h"
#include "InputRuntime.h"
#include "Messager.h"
#include "Gfx.h"
#include "Sfx.h"
#include "SchedulerRuntime.h"
#include "TypeObject.h"
#include "TypeComponent.h"
#include "TypeAction.h"
#include "ComponentPosition.h"
#include "ComponentPhysics.h"
#include "ComponentDraw.h"
#include "Camera.h"
#include "raylib.h"
#include "box2d/box2d.h"

namespace Engine
{
    namespace
    {
        static const char* LUA_METATABLE_OBJECT = "Minix.Object";
        static const char* LUA_METATABLE_COMPONENT = "Minix.Component";
        static const char* LUA_METATABLE_ACTION = "Minix.Action";
        static const char* LUA_METATABLE_CAMERA = "Minix.Camera";
        static const char* LUA_METATABLE_EVENT = "Minix.Event";

        static const char* LUA_TYPE_ACTION = "action";
        static const char* LUA_TYPE_OBJECT = "object";

        static std::vector<LuaComponentBinding> g_bindings;
        static std::vector<LuaCallableBinding> g_globalFunctions;
        static std::vector<LuaCallableBinding> g_objectMethods;
        static std::vector<LuaCallableBinding> g_actionMethods;
        static std::vector<LuaCallableBinding> g_cameraMethods;
        static std::vector<LuaConstantTableBinding> g_constantTables;
    }

    // =========================================================
    // Registration
    // =========================================================

    void LuaAPI::RegisterComponentBinding(const LuaComponentBinding& binding)
    {
        g_bindings.push_back(binding);
    }

    void LuaAPI::RegisterGlobalFunction(const LuaCallableBinding& binding)
    {
        g_globalFunctions.push_back(binding);
    }

    void LuaAPI::RegisterObjectMethod(const LuaCallableBinding& binding)
    {
        g_objectMethods.push_back(binding);
    }

    void LuaAPI::RegisterActionMethod(const LuaCallableBinding& binding)
    {
        g_actionMethods.push_back(binding);
    }

    void LuaAPI::RegisterCameraMethod(const LuaCallableBinding& binding)
    {
        g_cameraMethods.push_back(binding);
    }

    void LuaAPI::RegisterConstantTable(const LuaConstantTableBinding& binding)
    {
        g_constantTables.push_back(binding);
    }

    // =========================================================
    // Context
    // =========================================================

    bool LuaAPI::IsAllowedInCurrentContext(int allowedMask)
    {
        const LuaExecutionContext ctx = LuaRuntime::GetCurrentContext();

        switch (ctx)
        {
        case LuaExecutionContext::Load:   return (allowedMask & LuaCtxLoad) != 0;
        case LuaExecutionContext::Update: return (allowedMask & LuaCtxUpdate) != 0;
        case LuaExecutionContext::Draw:   return (allowedMask & LuaCtxDraw) != 0;
        default:                         return (allowedMask & LuaCtxOutside) != 0;
        }
    }

    int LuaAPI::ErrorContextNotAllowed(lua_State* L, const char* apiName, int allowedMask)
    {
        char allowed[128] = "";
        bool first = true;

        auto append = [&](const char* name)
            {
                if (!first) std::strcat(allowed, ", ");
                std::strcat(allowed, name);
                first = false;
            };

        if (allowedMask & LuaCtxLoad) append("minix.load");
        if (allowedMask & LuaCtxUpdate) append("minix.update");
        if (allowedMask & LuaCtxDraw) append("minix.draw");
        if (allowedMask & LuaCtxOutside) append("outside");

        return luaL_error(
            L,
            "Lua API '%s' is not allowed in context '%s' (allowed: %s)",
            apiName ? apiName : "<unknown>",
            LuaRuntime::GetCurrentContextName(),
            allowed[0] ? allowed : "<none>"
        );
    }

    // =========================================================
    // Helpers
    // =========================================================

    Object* LuaAPI::FindObjectById(int id)
    {
        for (auto* object : Registry::GetAllObjects())
        {
            if (object && object->GetID() == id)
                return object;
        }

        return nullptr;
    }

    int LuaAPI::GetObjectIdFromProxy(lua_State* L, int index)
    {
        if (!lua_istable(L, index)) return -1;

        lua_getfield(L, index, "_id");
        if (!lua_isinteger(L, -1))
        {
            lua_pop(L, 1);
            return -1;
        }

        int id = static_cast<int>(lua_tointeger(L, -1));
        lua_pop(L, 1);
        return id;
    }

    bool LuaAPI::GetDescriptorType(lua_State* L, int index, const char** outType)
    {
        if (lua_isstring(L, index))
        {
            *outType = lua_tostring(L, index);
            return true;
        }

        if (!lua_istable(L, index)) return false;

        lua_getfield(L, index, "_descriptorType");
        if (!lua_isstring(L, -1))
        {
            lua_pop(L, 1);
            return false;
        }

        *outType = lua_tostring(L, -1);
        lua_pop(L, 1);
        return true;
    }

    bool LuaAPI::GetComponentProxyInfo(lua_State* L, int index, int* outOwnerId, const char** outType)
    {
        if (!lua_istable(L, index)) return false;

        lua_getfield(L, index, "_ownerId");
        if (!lua_isinteger(L, -1))
        {
            lua_pop(L, 1);
            return false;
        }
        *outOwnerId = static_cast<int>(lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_getfield(L, index, "_componentType");
        if (!lua_isstring(L, -1))
        {
            lua_pop(L, 1);
            return false;
        }
        *outType = lua_tostring(L, -1);
        lua_pop(L, 1);

        return true;
    }

    const LuaComponentBinding* LuaAPI::FindBinding(const char* luaName)
    {
        for (auto& binding : g_bindings)
        {
            if (binding.luaName && std::strcmp(binding.luaName, luaName) == 0)
                return &binding;
        }

        return nullptr;
    }

    const LuaCallableBinding* LuaAPI::FindObjectMethod(const char* name)
    {
        for (auto& method : g_objectMethods)
        {
            if (method.name && std::strcmp(method.name, name) == 0)
                return &method;
        }

        return nullptr;
    }

    const LuaCallableBinding* LuaAPI::FindActionMethod(const char* name)
    {
        for (auto& method : g_actionMethods)
        {
            if (method.name && std::strcmp(method.name, name) == 0)
                return &method;
        }

        return nullptr;
    }

    const LuaCallableBinding* LuaAPI::FindCameraMethod(const char* name)
    {
        for (auto& method : g_cameraMethods)
        {
            if (method.name && std::strcmp(method.name, name) == 0)
                return &method;
        }

        return nullptr;
    }

    // =========================================================
    // Color helpers
    // =========================================================

    void LuaAPI::PushColorTable(lua_State* L, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        lua_newtable(L);

        lua_pushinteger(L, r);
        lua_setfield(L, -2, "r");

        lua_pushinteger(L, g);
        lua_setfield(L, -2, "g");

        lua_pushinteger(L, b);
        lua_setfield(L, -2, "b");

        lua_pushinteger(L, a);
        lua_setfield(L, -2, "a");
    }

    void LuaAPI::PushColorTableFromRaylib(lua_State* L, Color color)
    {
        PushColorTable(L, color.r, color.g, color.b, color.a);
    }

    bool LuaAPI::ReadColorTable(lua_State* L, int index, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a)
    {
        if (!lua_istable(L, index)) return false;

        lua_getfield(L, index, "r");
        if (!lua_isnumber(L, -1)) { lua_pop(L, 1); return false; }
        r = static_cast<unsigned char>(lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_getfield(L, index, "g");
        if (!lua_isnumber(L, -1)) { lua_pop(L, 1); return false; }
        g = static_cast<unsigned char>(lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_getfield(L, index, "b");
        if (!lua_isnumber(L, -1)) { lua_pop(L, 1); return false; }
        b = static_cast<unsigned char>(lua_tointeger(L, -1));
        lua_pop(L, 1);

        lua_getfield(L, index, "a");
        if (!lua_isnumber(L, -1)) { lua_pop(L, 1); return false; }
        a = static_cast<unsigned char>(lua_tointeger(L, -1));
        lua_pop(L, 1);

        return true;
    }

    // =========================================================
    // Proxies
    // =========================================================

    void LuaAPI::PushObjectProxy(lua_State* L, Object* object)
    {
        if (!object)
        {
            lua_pushnil(L);
            return;
        }

        lua_newtable(L);

        lua_pushinteger(L, object->GetID());
        lua_setfield(L, -2, "_id");

        luaL_getmetatable(L, LUA_METATABLE_OBJECT);
        lua_setmetatable(L, -2);
    }

    void LuaAPI::PushComponentProxy(lua_State* L, Object* owner, const char* type)
    {
        if (!owner || !type)
        {
            lua_pushnil(L);
            return;
        }

        const LuaComponentBinding* binding = FindBinding(type);
        if (!binding || !binding->get || !binding->get(owner))
        {
            lua_pushnil(L);
            return;
        }

        lua_newtable(L);

        lua_pushinteger(L, owner->GetID());
        lua_setfield(L, -2, "_ownerId");

        lua_pushstring(L, type);
        lua_setfield(L, -2, "_componentType");

        luaL_getmetatable(L, LUA_METATABLE_COMPONENT);
        lua_setmetatable(L, -2);
    }

    Action* LuaAPI::GetActionFromProxy(lua_State* L, int index)
    {
        if (!lua_istable(L, index)) return nullptr;

        lua_getfield(L, index, "_ptr");
        if (!lua_islightuserdata(L, -1))
        {
            lua_pop(L, 1);
            return nullptr;
        }

        Action* action = static_cast<Action*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        return action;
    }

    void LuaAPI::PushActionProxy(lua_State* L, Action* action)
    {
        if (!action)
        {
            lua_pushnil(L);
            return;
        }

        lua_newtable(L);

        lua_pushlightuserdata(L, action);
        lua_setfield(L, -2, "_ptr");

        luaL_getmetatable(L, LUA_METATABLE_ACTION);
        lua_setmetatable(L, -2);
    }

    TypeEvent* LuaAPI::GetEventFromProxy(lua_State* L, int index)
    {
        if (!lua_istable(L, index)) return nullptr;

        lua_getfield(L, index, "_ptr");
        if (!lua_islightuserdata(L, -1))
        {
            lua_pop(L, 1);
            return nullptr;
        }

        TypeEvent* eventRef = static_cast<TypeEvent*>(lua_touserdata(L, -1));
        lua_pop(L, 1);
        return eventRef;
    }

    void LuaAPI::PushEventProxy(lua_State* L, TypeEvent* eventRef)
    {
        if (!eventRef)
        {
            lua_pushnil(L);
            return;
        }

        lua_newtable(L);

        lua_pushlightuserdata(L, eventRef);
        lua_setfield(L, -2, "_ptr");

        luaL_getmetatable(L, LUA_METATABLE_EVENT);
        lua_setmetatable(L, -2);
    }

    // =========================================================
    // Snapshot helpers
    // =========================================================

    int LuaAPI::Lua_SnapshotIterator(lua_State* L)
    {
        int index = 0;
        if (!lua_isnil(L, 2))
            index = static_cast<int>(lua_tointeger(L, 2));

        lua_pushvalue(L, lua_upvalueindex(1));
        lua_rawgeti(L, -1, index + 1);

        if (lua_isnil(L, -1))
            return 0;

        lua_pushinteger(L, index + 1);
        lua_insert(L, -2);
        return 2;
    }

    void LuaAPI::PushIteratorFromTopTable(lua_State* L)
    {
        lua_pushcclosure(L, Lua_SnapshotIterator, 1);
    }

    void LuaAPI::PushChildrenIterator(lua_State* L, Object* object)
    {
        lua_newtable(L);

        int i = 1;
        for (auto* child : object->GetChildren())
        {
            PushObjectProxy(L, child);
            lua_rawseti(L, -2, i++);
        }

        PushIteratorFromTopTable(L);
    }

    // =========================================================
    // Generic bound wrappers
    // =========================================================

    void LuaAPI::PushBoundGlobal(lua_State* L, int index)
    {
        lua_pushinteger(L, index);
        lua_pushcclosure(L, Lua_DispatchGlobal, 1);
    }

    void LuaAPI::PushBoundObjectMethod(lua_State* L, int index)
    {
        lua_pushinteger(L, index);
        lua_pushcclosure(L, Lua_DispatchObjectMethod, 1);
    }

    void LuaAPI::PushBoundActionMethod(lua_State* L, int index)
    {
        lua_pushinteger(L, index);
        lua_pushcclosure(L, Lua_DispatchActionMethod, 1);
    }

    void LuaAPI::PushBoundCameraMethod(lua_State* L, int index)
    {
        lua_pushinteger(L, index);
        lua_pushcclosure(L, Lua_DispatchCameraMethod, 1);
    }

    int LuaAPI::Lua_DispatchGlobal(lua_State* L)
    {
        int index = static_cast<int>(lua_tointeger(L, lua_upvalueindex(1)));
        if (index < 0 || index >= static_cast<int>(g_globalFunctions.size()))
            return 0;

        const auto& binding = g_globalFunctions[index];

        if (!IsAllowedInCurrentContext(binding.allowedMask))
            return ErrorContextNotAllowed(L, binding.name, binding.allowedMask);

        if (!binding.callback) return 0;
        return binding.callback(L);
    }

    int LuaAPI::Lua_DispatchObjectMethod(lua_State* L)
    {
        int index = static_cast<int>(lua_tointeger(L, lua_upvalueindex(1)));
        if (index < 0 || index >= static_cast<int>(g_objectMethods.size()))
            return 0;

        const auto& binding = g_objectMethods[index];

        if (!IsAllowedInCurrentContext(binding.allowedMask))
            return ErrorContextNotAllowed(L, binding.name, binding.allowedMask);

        if (!binding.callback) return 0;
        return binding.callback(L);
    }

    int LuaAPI::Lua_DispatchActionMethod(lua_State* L)
    {
        int index = static_cast<int>(lua_tointeger(L, lua_upvalueindex(1)));
        if (index < 0 || index >= static_cast<int>(g_actionMethods.size()))
            return 0;

        const auto& binding = g_actionMethods[index];

        if (!IsAllowedInCurrentContext(binding.allowedMask))
            return ErrorContextNotAllowed(L, binding.name, binding.allowedMask);

        if (!binding.callback) return 0;
        return binding.callback(L);
    }

    int LuaAPI::Lua_DispatchCameraMethod(lua_State* L)
    {
        int index = static_cast<int>(lua_tointeger(L, lua_upvalueindex(1)));
        if (index < 0 || index >= static_cast<int>(g_cameraMethods.size()))
            return 0;

        const auto& binding = g_cameraMethods[index];

        if (!IsAllowedInCurrentContext(binding.allowedMask))
            return ErrorContextNotAllowed(L, binding.name, binding.allowedMask);

        if (!binding.callback) return 0;
        return binding.callback(L);
    }

    // =========================================================
    // Event API
    // =========================================================

    int LuaAPI::Lua_EventBind(lua_State* L)
    {
        TypeEvent* eventRef = GetEventFromProxy(L, 1);
        if (!eventRef) return 0;
        if (!lua_isfunction(L, 2)) return 0;

        Messager::BindLua(*eventRef, L, 2);
        return 0;
    }

    int LuaAPI::Lua_EventIndex(lua_State* L)
    {
        const char* key = luaL_checkstring(L, 2);

        if (std::strcmp(key, "bind") == 0)
        {
            lua_pushcfunction(L, Lua_EventBind);
            return 1;
        }

        lua_pushnil(L);
        return 1;
    }

    // =========================================================
    // Action API
    // =========================================================

    int LuaAPI::Lua_ActionBindKeyDown(lua_State* L)
    {
        Action* action = GetActionFromProxy(L, 1);
        if (!action) return 0;

        int key = static_cast<int>(luaL_checkinteger(L, 2));
        InputRuntime::BindKeyDown(action, key);
        return 0;
    }

    int LuaAPI::Lua_ActionBindKeyUp(lua_State* L)
    {
        Action* action = GetActionFromProxy(L, 1);
        if (!action) return 0;

        int key = static_cast<int>(luaL_checkinteger(L, 2));
        InputRuntime::BindKeyUp(action, key);
        return 0;
    }

    int LuaAPI::Lua_ActionBindLogic(lua_State* L)
    {
        Action* action = GetActionFromProxy(L, 1);
        if (!action) return 0;
        if (!lua_isfunction(L, 2)) return 0;

        lua_pushvalue(L, 2);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);

        InputRuntime::BindLogic(action,
            [ref](float dt)
            {
                lua_State* state = LuaRuntime::GetState();
                if (!state) return;

                lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
                lua_pushnumber(state, dt);

                const LuaExecutionContext previous = LuaRuntime::GetCurrentContext();
                LuaRuntime::SetCurrentContext(LuaExecutionContext::Update);

                if (lua_pcall(state, 1, 0, 0) != LUA_OK)
                {
                    const char* err = lua_tostring(state, -1);
                    if (err) std::printf("Lua action error: %s\n", err);
                    lua_pop(state, 1);
                }

                LuaRuntime::SetCurrentContext(previous);
            }
        );

        return 0;
    }

    int LuaAPI::Lua_ActionIndex(lua_State* L)
    {
        const char* key = luaL_checkstring(L, 2);

        for (int i = 0; i < static_cast<int>(g_actionMethods.size()); ++i)
        {
            if (g_actionMethods[i].name && std::strcmp(g_actionMethods[i].name, key) == 0)
            {
                PushBoundActionMethod(L, i);
                return 1;
            }
        }

        lua_pushnil(L);
        return 1;
    }

    // =========================================================
    // Object API
    // =========================================================

    int LuaAPI::Lua_ObjectAddComponent(lua_State* L)
    {
        int objectId = GetObjectIdFromProxy(L, 1);
        Object* object = FindObjectById(objectId);
        if (!object)
        {
            lua_pushnil(L);
            return 1;
        }

        const char* type = nullptr;
        if (!GetDescriptorType(L, 2, &type))
        {
            lua_pushnil(L);
            return 1;
        }

        const LuaComponentBinding* binding = FindBinding(type);
        if (!binding || !binding->create)
        {
            lua_pushnil(L);
            return 1;
        }

        Component* component = binding->create(object);
        RefreshGameTables(L);

        if (!component)
        {
            lua_pushnil(L);
            return 1;
        }
        PushComponentProxy(L, object, type);
        return 1;
    }

    int LuaAPI::Lua_ObjectRemoveComponent(lua_State* L)
    {
        int objectId = GetObjectIdFromProxy(L, 1);
        Object* object = FindObjectById(objectId);
        if (!object) return 0;

        const char* type = nullptr;
        if (!GetDescriptorType(L, 2, &type)) return 0;

        const LuaComponentBinding* binding = FindBinding(type);
        if (binding && binding->destroy)
        {
            binding->destroy(object);
            RefreshGameTables(L);
        }

        return 0;
    }

    int LuaAPI::Lua_ObjectIndex(lua_State* L)
    {
        int objectId = GetObjectIdFromProxy(L, 1);
        Object* object = FindObjectById(objectId);
        if (!object)
        {
            lua_pushnil(L);
            return 1;
        }

        const char* key = luaL_checkstring(L, 2);

        if (std::strcmp(key, "id") == 0)
        {
            lua_pushinteger(L, object->GetID());
            return 1;
        }

        if (std::strcmp(key, "parent") == 0)
        {
            PushObjectProxy(L, object->GetParent());
            return 1;
        }

        if (std::strcmp(key, "children") == 0)
        {
            PushChildrenIterator(L, object);
            return 1;
        }

        for (int i = 0; i < static_cast<int>(g_objectMethods.size()); ++i)
        {
            if (g_objectMethods[i].name && std::strcmp(g_objectMethods[i].name, key) == 0)
            {
                PushBoundObjectMethod(L, i);
                return 1;
            }
        }

        const LuaComponentBinding* binding = FindBinding(key);
        if (binding)
        {
            PushComponentProxy(L, object, key);
            return 1;
        }

        lua_pushnil(L);
        return 1;
    }

    // =========================================================
    // Component API
    // =========================================================

    int LuaAPI::Lua_ComponentIndex(lua_State* L)
    {
        int ownerId = -1;
        const char* type = nullptr;

        if (!GetComponentProxyInfo(L, 1, &ownerId, &type))
        {
            lua_pushnil(L);
            return 1;
        }

        Object* owner = FindObjectById(ownerId);
        if (!owner)
        {
            lua_pushnil(L);
            return 1;
        }

        const char* field = luaL_checkstring(L, 2);

        if (std::strcmp(field, "parent") == 0)
        {
            PushObjectProxy(L, owner);
            return 1;
        }

        const LuaComponentBinding* binding = FindBinding(type);
        if (!binding)
        {
            lua_pushnil(L);
            return 1;
        }

        for (const auto& entry : binding->fields)
        {
            if (entry.name && std::strcmp(entry.name, field) == 0)
            {
                if (!IsAllowedInCurrentContext(entry.readMask))
                    return ErrorContextNotAllowed(L, field, entry.readMask);

                if (entry.push)
                    return entry.push(L, owner);

                lua_pushnil(L);
                return 1;
            }
        }

        for (const auto& eventEntry : binding->events)
        {
            if (eventEntry.name && std::strcmp(eventEntry.name, field) == 0)
            {
                if (eventEntry.getEvent)
                {
                    PushEventProxy(L, eventEntry.getEvent(owner));
                    return 1;
                }

                lua_pushnil(L);
                return 1;
            }
        }

        for (const auto& method : binding->methods)
        {
            if (method.name && std::strcmp(method.name, field) == 0)
            {
                PushComponentMethod(L, ownerId, type, method.name);
                return 1;
            }
        }

        if (binding->pushField)
            return binding->pushField(L, owner, field);

        lua_pushnil(L);
        return 1;
    }

    int LuaAPI::Lua_ComponentNewIndex(lua_State* L)
    {
        int ownerId = -1;
        const char* type = nullptr;

        if (!GetComponentProxyInfo(L, 1, &ownerId, &type))
            return 0;

        Object* owner = FindObjectById(ownerId);
        if (!owner)
            return 0;

        const char* field = luaL_checkstring(L, 2);

        const LuaComponentBinding* binding = FindBinding(type);
        if (!binding)
            return 0;

        for (const auto& entry : binding->fields)
        {
            if (entry.name && std::strcmp(entry.name, field) == 0)
            {
                if (!IsAllowedInCurrentContext(entry.writeMask))
                    return ErrorContextNotAllowed(L, field, entry.writeMask);

                if (entry.set)
                {
                    entry.set(L, owner, 3);
                    RefreshGameTables(L);
                }
                return 0;
            }
        }

        if (binding->setField)
        {
            binding->setField(L, owner, field, 3);
            RefreshGameTables(L);
        }

        return 0;
    }

    void LuaAPI::PushComponentMethod(lua_State* L, int ownerId, const char* type, const char* methodName)
    {
        lua_pushinteger(L, ownerId);
        lua_pushstring(L, type);
        lua_pushstring(L, methodName);
        lua_pushcclosure(L, Lua_ComponentMethodDispatch, 3);
    }

    int LuaAPI::Lua_ComponentMethodDispatch(lua_State* L)
    {
        int ownerId = static_cast<int>(luaL_checkinteger(L, lua_upvalueindex(1)));
        const char* type = lua_tostring(L, lua_upvalueindex(2));
        const char* methodName = lua_tostring(L, lua_upvalueindex(3));

        if (!type || !methodName)
            return 0;

        Object* owner = FindObjectById(ownerId);
        if (!owner)
            return 0;

        const LuaComponentBinding* binding = FindBinding(type);
        if (!binding)
            return 0;

        for (const auto& method : binding->methods)
        {
            if (method.name && std::strcmp(method.name, methodName) == 0)
            {
                if (!IsAllowedInCurrentContext(method.callMask))
                    return ErrorContextNotAllowed(L, methodName, method.callMask);

                if (method.call)
                    return method.call(L, owner);

                return 0;
            }
        }

        lua_pushnil(L);
        return 1;
    }

    // =========================================================
    // Global API
    // =========================================================

    int LuaAPI::Lua_New(lua_State* L)
    {
        const char* type = nullptr;
        if (!GetDescriptorType(L, 1, &type))
        {
            lua_pushnil(L);
            return 1;
        }

        if (std::strcmp(type, LUA_TYPE_OBJECT) == 0)
        {
            Object* object = Registry::CreateObject();
            RefreshGameTables(L);
            PushObjectProxy(L, object);
            return 1;
        }

        if (std::strcmp(type, LUA_TYPE_ACTION) == 0)
        {
            Action* action = InputRuntime::CreateAction();
            PushActionProxy(L, action);
            return 1;
        }

        lua_newtable(L);
        lua_pushstring(L, type);
        lua_setfield(L, -2, "_descriptorType");
        return 1;
    }

    int LuaAPI::Lua_Remove(lua_State* L)
    {
        if (lua_istable(L, 1))
        {
            int objectId = GetObjectIdFromProxy(L, 1);
            if (objectId >= 0)
            {
                Object* object = FindObjectById(objectId);
                if (object)
                {
                    Registry::DestroyObject(object);
                    RefreshGameTables(L);
                }
                return 0;
            }

            int ownerId = -1;
            const char* type = nullptr;
            if (GetComponentProxyInfo(L, 1, &ownerId, &type))
            {
                Object* owner = FindObjectById(ownerId);
                if (!owner) return 0;

                const LuaComponentBinding* binding = FindBinding(type);
                if (!binding || !binding->get) return 0;

                Component* component = binding->get(owner);
                if (component)
                {
                    Registry::DestroyComponent(component);
                    RefreshGameTables(L);
                }
                return 0;
            }
        }

        return 0;
    }

    int LuaAPI::Lua_After(lua_State* L)
    {
        float seconds = static_cast<float>(luaL_checknumber(L, 1));
        luaL_checktype(L, 2, LUA_TFUNCTION);

        lua_pushvalue(L, 2);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);

        SchedulerRuntime::After(seconds, ref);
        return 0;
    }

    int LuaAPI::Lua_Every(lua_State* L)
    {
        float seconds = static_cast<float>(luaL_checknumber(L, 1));
        luaL_checktype(L, 2, LUA_TFUNCTION);

        lua_pushvalue(L, 2);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);

        SchedulerRuntime::Every(seconds, ref);
        return 0;
    }

    int LuaAPI::Lua_Play(lua_State* L)
    {
        int id = static_cast<int>(luaL_checkinteger(L, 1));
        Sfx::Play(id);
        return 0;
    }

    int LuaAPI::Lua_DrawSprite(lua_State* L)
    {
        int gfxId = static_cast<int>(luaL_checkinteger(L, 1));
        float x = static_cast<float>(luaL_checknumber(L, 2));
        float y = static_cast<float>(luaL_checknumber(L, 3));
        float w = static_cast<float>(luaL_checknumber(L, 4));
        float h = static_cast<float>(luaL_checknumber(L, 5));

        Texture2D* texture = Gfx::Get(gfxId);
        if (!texture) return 0;

        Color tint = WHITE;

        if (lua_gettop(L) >= 6 && lua_istable(L, 6))
        {
            unsigned char r, g, b, a;
            if (LuaAPI::ReadColorTable(L, 6, r, g, b, a))
                tint = Color{ r, g, b, a };
        }

        Rectangle src{
            0.0f,
            0.0f,
            static_cast<float>(texture->width),
            static_cast<float>(texture->height)
        };

        Rectangle dst{
            x,
            y,
            w,
            h
        };

        Vector2 origin{
            w / 2.0f,
            h / 2.0f
        };

        DrawTexturePro(*texture, src, dst, origin, 0.0f, tint);
        return 0;
    }

    // =========================================================
    // Camera API
    // =========================================================

    int LuaAPI::Lua_CameraBindTarget(lua_State* L)
    {
        int ownerId = -1;
        const char* type = nullptr;

        if (!GetComponentProxyInfo(L, 2, &ownerId, &type))
            return 0;

        if (std::strcmp(type, "position2D") != 0)
            return 0;

        Object* owner = FindObjectById(ownerId);
        if (!owner)
            return 0;

        auto* pos = owner->GetComponent<Position2D>();
        if (!pos)
            return 0;

        Camera::BindTarget(pos);
        return 0;
    }

    int LuaAPI::Lua_CameraUnbindTarget(lua_State* L)
    {
        (void)L;
        Camera::UnbindTarget();
        return 0;
    }

    int LuaAPI::Lua_CameraIndex(lua_State* L)
    {
        const char* key = luaL_checkstring(L, 2);

        if (std::strcmp(key, "x") == 0)
        {
            lua_pushnumber(L, Camera::GetX());
            return 1;
        }

        if (std::strcmp(key, "y") == 0)
        {
            lua_pushnumber(L, Camera::GetY());
            return 1;
        }

        if (std::strcmp(key, "a") == 0)
        {
            lua_pushnumber(L, Camera::GetA());
            return 1;
        }

        if (std::strcmp(key, "zoom") == 0)
        {
            lua_pushnumber(L, Camera::GetZoom());
            return 1;
        }

        for (int i = 0; i < static_cast<int>(g_cameraMethods.size()); ++i)
        {
            if (g_cameraMethods[i].name && std::strcmp(g_cameraMethods[i].name, key) == 0)
            {
                PushBoundCameraMethod(L, i);
                return 1;
            }
        }

        lua_pushnil(L);
        return 1;
    }

    int LuaAPI::Lua_CameraNewIndex(lua_State* L)
    {
        const char* key = luaL_checkstring(L, 2);

        if (std::strcmp(key, "x") == 0)
        {
            Camera::SetX(static_cast<float>(luaL_checknumber(L, 3)));
            return 0;
        }

        if (std::strcmp(key, "y") == 0)
        {
            Camera::SetY(static_cast<float>(luaL_checknumber(L, 3)));
            return 0;
        }

        if (std::strcmp(key, "a") == 0)
        {
            Camera::SetA(static_cast<float>(luaL_checknumber(L, 3)));
            return 0;
        }

        if (std::strcmp(key, "zoom") == 0)
        {
            Camera::SetZoom(static_cast<float>(luaL_checknumber(L, 3)));
            return 0;
        }

        return 0;
    }

    // =========================================================
    // Refresh
    // =========================================================

    void LuaAPI::RefreshGameObjects(lua_State* L)
    {
        lua_getglobal(L, "game");
        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1);
            return;
        }

        lua_newtable(L);

        int i = 1;
        for (auto* object : Registry::GetAllObjects())
        {
            PushObjectProxy(L, object);
            lua_rawseti(L, -2, i++);
        }

        PushIteratorFromTopTable(L);
        lua_setfield(L, -2, "objects");

        lua_pop(L, 1);
    }

    void LuaAPI::RefreshGameComponents(lua_State* L)
    {
        lua_getglobal(L, "game");
        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1);
            return;
        }

        lua_getfield(L, -1, "components");
        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setfield(L, -2, "components");
            lua_getfield(L, -1, "components");
        }

        std::vector<const char*> groups;

        auto addGroup = [&](const char* name)
            {
                if (!name) return;

                for (auto* group : groups)
                {
                    if (std::strcmp(group, name) == 0)
                        return;
                }

                groups.push_back(name);
            };

        for (auto& binding : g_bindings)
        {
            addGroup(binding.groupA);
            addGroup(binding.groupB);
        }

        for (auto* groupName : groups)
        {
            lua_newtable(L);
            int nextIndex = 1;
            int tableIndex = lua_gettop(L);

            for (auto& binding : g_bindings)
            {
                bool belongs =
                    (binding.groupA && std::strcmp(binding.groupA, groupName) == 0) ||
                    (binding.groupB && std::strcmp(binding.groupB, groupName) == 0);

                if (!belongs) continue;
                if (!binding.appendSnapshot) continue;

                binding.appendSnapshot(L, tableIndex, &nextIndex);
            }

            PushIteratorFromTopTable(L);
            lua_setfield(L, -2, groupName);
        }

        lua_pop(L, 2);
    }

    void LuaAPI::RefreshGameTables(lua_State* L)
    {
        RefreshGameObjects(L);
        RefreshGameComponents(L);
    }

    void LuaAPI::Refresh(lua_State* L)
    {
        RefreshGameTables(L);
    }

    // =========================================================
    // Metatables and core tables
    // =========================================================

    void LuaAPI::RegisterMetatables(lua_State* L)
    {
        luaL_newmetatable(L, LUA_METATABLE_OBJECT);
        lua_pushcfunction(L, Lua_ObjectIndex);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);

        luaL_newmetatable(L, LUA_METATABLE_COMPONENT);
        lua_pushcfunction(L, Lua_ComponentIndex);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, Lua_ComponentNewIndex);
        lua_setfield(L, -2, "__newindex");
        lua_pop(L, 1);

        luaL_newmetatable(L, LUA_METATABLE_ACTION);
        lua_pushcfunction(L, Lua_ActionIndex);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);

        luaL_newmetatable(L, LUA_METATABLE_CAMERA);
        lua_pushcfunction(L, Lua_CameraIndex);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, Lua_CameraNewIndex);
        lua_setfield(L, -2, "__newindex");
        lua_pop(L, 1);

        luaL_newmetatable(L, LUA_METATABLE_EVENT);
        lua_pushcfunction(L, Lua_EventIndex);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);
    }

    void LuaAPI::CreateCoreTables(lua_State* L)
    {
        lua_newtable(L);
        lua_setglobal(L, "minix");

        lua_newtable(L);
        lua_newtable(L);
        lua_setfield(L, -2, "components");
        lua_setglobal(L, "game");

        lua_newtable(L);
        luaL_getmetatable(L, LUA_METATABLE_CAMERA);
        lua_setmetatable(L, -2);
        lua_setglobal(L, "camera2D");

        for (int i = 0; i < static_cast<int>(g_globalFunctions.size()); ++i)
        {
            PushBoundGlobal(L, i);
            lua_setglobal(L, g_globalFunctions[i].name);
        }
    }

    void LuaAPI::CreateTypeDescriptors(lua_State* L)
    {
        auto pushDescriptor = [&](const char* name)
            {
                lua_newtable(L);
                lua_pushstring(L, name);
                lua_setfield(L, -2, "_descriptorType");
                lua_setglobal(L, name);
            };

        pushDescriptor(LUA_TYPE_OBJECT);
        pushDescriptor(LUA_TYPE_ACTION);

        for (auto& binding : g_bindings)
            pushDescriptor(binding.luaName);
    }

    void LuaAPI::CreateConstantTables(lua_State* L)
    {
        for (const auto& table : g_constantTables)
        {
            lua_newtable(L);

            for (const auto& entry : table.entries)
            {
                switch (entry.kind)
                {
                case LuaConstantKind::Integer:
                    lua_pushinteger(L, static_cast<lua_Integer>(entry.integerValue));
                    break;

                case LuaConstantKind::Number:
                    lua_pushnumber(L, static_cast<lua_Number>(entry.numberValue));
                    break;

                case LuaConstantKind::Boolean:
                    lua_pushboolean(L, entry.boolValue ? 1 : 0);
                    break;

                case LuaConstantKind::String:
                    lua_pushstring(L, entry.stringValue ? entry.stringValue : "");
                    break;

                case LuaConstantKind::Color:
                    LuaAPI::PushColorTableFromRaylib(L, entry.colorValue);
                    break;
                }

                lua_setfield(L, -2, entry.name);
            }

            lua_setglobal(L, table.tableName);
        }
    }

    void LuaAPI::CreateAssetTables(lua_State* L)
    {
        lua_newtable(L);
        for (int i = 0; i < Gfx::GetCount(); ++i)
        {
            const char* name = Gfx::GetName(i);
            if (!name) continue;

            lua_pushinteger(L, i);
            lua_setfield(L, -2, name);
        }
        lua_setglobal(L, "gfx");

        lua_newtable(L);
        for (int i = 0; i < Sfx::GetCount(); ++i)
        {
            const char* name = Sfx::GetName(i);
            if (!name) continue;

            lua_pushinteger(L, i);
            lua_setfield(L, -2, name);
        }
        lua_setglobal(L, "sfx");
    }

    // =========================================================
    // Builtins registration
    // =========================================================

    void LuaAPI::RegisterBindings()
    {
        g_bindings.clear();
        g_globalFunctions.clear();
        g_objectMethods.clear();
        g_actionMethods.clear();
        g_cameraMethods.clear();
        g_constantTables.clear();

        Position2D::RegisterLuaBinding();
        Force2D::RegisterLuaBinding();
        CirclePhysics2D::RegisterLuaBinding();
        RectanglePhysics2D::RegisterLuaBinding();
        CircleDraw2D::RegisterLuaBinding();
        RectangleDraw2D::RegisterLuaBinding();
        SpriteDraw2D::RegisterLuaBinding();

        RegisterBuiltins();
    }

    void LuaAPI::RegisterBuiltins()
    {
        LuaGlobalFunctionBuilder("new")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_New)
            .Register();

        LuaGlobalFunctionBuilder("remove")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_Remove)
            .Register();

        LuaGlobalFunctionBuilder("after")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_After)
            .Register();

        LuaGlobalFunctionBuilder("every")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_Every)
            .Register();

        LuaGlobalFunctionBuilder("play")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(true)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_Play)
            .Register();

        LuaGlobalFunctionBuilder("drawSprite")
            .AllowInLoad(false)
            .AllowInUpdate(false)
            .AllowInDraw(true)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_DrawSprite)
            .Register();

        LuaObjectMethodBuilder("addComponent")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_ObjectAddComponent)
            .Register();

        LuaObjectMethodBuilder("removeComponent")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_ObjectRemoveComponent)
            .Register();

        LuaActionMethodBuilder("bindKeyDown")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_ActionBindKeyDown)
            .Register();

        LuaActionMethodBuilder("bindKeyUp")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_ActionBindKeyUp)
            .Register();

        LuaActionMethodBuilder("bindLogic")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_ActionBindLogic)
            .Register();

        LuaCameraMethodBuilder("bindTarget")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_CameraBindTarget)
            .Register();

        LuaCameraMethodBuilder("unbindTarget")
            .AllowInLoad(true)
            .AllowInUpdate(true)
            .AllowInDraw(false)
            .AllowOutside(false)
            .Bind(&LuaAPI::Lua_CameraUnbindTarget)
            .Register();

        LuaConstantTableBuilder("key")
            .Int("space", KEY_SPACE)
            .Int("left", KEY_LEFT)
            .Int("right", KEY_RIGHT)
            .Int("up", KEY_UP)
            .Int("down", KEY_DOWN)
            .Int("a", KEY_A)
            .Int("d", KEY_D)
            .Int("w", KEY_W)
            .Int("s", KEY_S)
            .Int("z", KEY_Z)
            .Int("x", KEY_X)
            .Int("q", KEY_Q)
            .Int("e", KEY_E)
            .Int("num0", KEY_ZERO)
            .Int("num1", KEY_ONE)
            .Int("num2", KEY_TWO)
            .Int("num3", KEY_THREE)
            .Int("num4", KEY_FOUR)
            .Int("num5", KEY_FIVE)
            .Int("num6", KEY_SIX)
            .Int("num7", KEY_SEVEN)
            .Int("num8", KEY_EIGHT)
            .Int("num9", KEY_NINE)
            .Register();

        LuaConstantTableBuilder("color")
            .Color("white", WHITE)
            .Color("black", BLACK)
            .Color("gray", GRAY)
            .Color("lightgray", LIGHTGRAY)
            .Color("darkgray", DARKGRAY)
            .Color("red", RED)
            .Color("maroon", MAROON)
            .Color("green", GREEN)
            .Color("lime", LIME)
            .Color("darkgreen", DARKGREEN)
            .Color("blue", BLUE)
            .Color("darkblue", DARKBLUE)
            .Color("skyblue", SKYBLUE)
            .Color("yellow", YELLOW)
            .Color("gold", GOLD)
            .Color("orange", ORANGE)
            .Color("pink", PINK)
            .Color("purple", PURPLE)
            .Color("violet", VIOLET)
            .Color("beige", BEIGE)
            .Color("brown", BROWN)
            .Color("darkbrown", DARKBROWN)
            .Register();
    }

    // =========================================================
    // Init
    // =========================================================

    void LuaAPI::Init(lua_State* L)
    {
        RegisterBindings();
        RegisterMetatables(L);
        CreateCoreTables(L);
        CreateTypeDescriptors(L);
        CreateConstantTables(L);
        CreateAssetTables(L);
        Refresh(L);
    }
}