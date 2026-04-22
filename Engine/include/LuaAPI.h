#pragma once

#include <vector>
#include <functional>
#include <utility>
#include <type_traits>
#include <cstring>

#include "raylib.h"
#include "Registry.h"
#include "ComponentDraw.h"
#include "ComponentPhysics.h"
#include "ComponentPosition.h"
#include "TypeEvent.h"

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace Engine
{
    class Object;
    class Component;
    class Action;

    enum LuaContextMask
    {
        LuaCtxNone = 0,
        LuaCtxLoad = 1 << 0,
        LuaCtxUpdate = 1 << 1,
        LuaCtxDraw = 1 << 2,
        LuaCtxOutside = 1 << 3,
        LuaCtxAny = LuaCtxLoad | LuaCtxUpdate | LuaCtxDraw | LuaCtxOutside
    };

    struct LuaFieldBinding
    {
        const char* name = nullptr;
        int readMask = LuaCtxAny;
        int writeMask = LuaCtxAny;
        std::function<int(lua_State*, Object*)> push;
        std::function<bool(lua_State*, Object*, int)> set;
    };

    struct LuaEventFieldBinding
    {
        const char* name = nullptr;
        std::function<TypeEvent* (Object*)> getEvent;
    };

    struct LuaMethodBinding
    {
        const char* name = nullptr;
        int callMask = LuaCtxAny;
        std::function<int(lua_State*, Object*)> call;
    };

    struct LuaComponentBinding
    {
        const char* luaName = nullptr;
        const char* groupA = nullptr;
        const char* groupB = nullptr;

        std::function<Component* (Object*)> get;
        std::function<Component* (Object*)> create;
        std::function<void(Object*)> destroy;

        std::function<void(lua_State*, int, int*)> appendSnapshot;

        std::function<int(lua_State*, Object*, const char*)> pushField;
        std::function<bool(lua_State*, Object*, const char*, int)> setField;

        std::vector<LuaFieldBinding> fields;
        std::vector<LuaMethodBinding> methods;
        std::vector<LuaEventFieldBinding> events;
    };

    struct LuaCallableBinding
    {
        const char* name = nullptr;
        int allowedMask = LuaCtxAny;
        std::function<int(lua_State*)> callback;
    };

    enum class LuaConstantKind
    {
        Integer,
        Number,
        Boolean,
        String,
        Color
    };

    struct LuaConstantEntry
    {
        LuaConstantKind kind = LuaConstantKind::Integer;
        const char* name = nullptr;
        long long integerValue = 0;
        double numberValue = 0.0;
        bool boolValue = false;
        const char* stringValue = nullptr;
        Color colorValue = Color{ 0, 0, 0, 255 };
    };

    struct LuaConstantTableBinding
    {
        const char* tableName = nullptr;
        std::vector<LuaConstantEntry> entries;
    };

    class LuaAPI
    {
    public:
        static void Init(lua_State* L);
        static void Refresh(lua_State* L);

        static void RegisterComponentBinding(const LuaComponentBinding& binding);
        static void RegisterGlobalFunction(const LuaCallableBinding& binding);
        static void RegisterObjectMethod(const LuaCallableBinding& binding);
        static void RegisterActionMethod(const LuaCallableBinding& binding);
        static void RegisterCameraMethod(const LuaCallableBinding& binding);
        static void RegisterConstantTable(const LuaConstantTableBinding& binding);

        static void PushObjectProxy(lua_State* L, Object* object);
        static void PushComponentProxy(lua_State* L, Object* owner, const char* type);
        static void PushEventProxy(lua_State* L, TypeEvent* eventRef);
        static TypeEvent* GetEventFromProxy(lua_State* L, int index);
        static void PushColorTable(lua_State* L, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        static void PushColorTableFromRaylib(lua_State* L, Color color);
        static bool ReadColorTable(lua_State* L, int index, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a);
        static bool GetComponentProxyInfo(lua_State* L, int index, int* outOwnerId, const char** outType);

    private:
        static void RegisterBuiltins();

        static void CreateCoreTables(lua_State* L);
        static void CreateTypeDescriptors(lua_State* L);
        static void CreateConstantTables(lua_State* L);
        static void RegisterMetatables(lua_State* L);
        static void RegisterBindings();

        static void RefreshGameObjects(lua_State* L);
        static void RefreshGameComponents(lua_State* L);
        static void RefreshGameTables(lua_State* L);

        static Object* FindObjectById(int id);
        static int GetObjectIdFromProxy(lua_State* L, int index);
        static bool GetDescriptorType(lua_State* L, int index, const char** outType);
        static const LuaComponentBinding* FindBinding(const char* luaName);

        static void PushComponentMethod(lua_State* L, int ownerId, const char* type, const char* methodName);
        static int Lua_ComponentMethodDispatch(lua_State* L);

        static const LuaCallableBinding* FindObjectMethod(const char* name);
        static const LuaCallableBinding* FindActionMethod(const char* name);
        static const LuaCallableBinding* FindCameraMethod(const char* name);

        static bool IsAllowedInCurrentContext(int allowedMask);
        static int ErrorContextNotAllowed(lua_State* L, const char* apiName, int allowedMask);

        static void PushIteratorFromTopTable(lua_State* L);
        static void PushChildrenIterator(lua_State* L, Object* object);

        static void PushBoundGlobal(lua_State* L, int index);
        static void PushBoundObjectMethod(lua_State* L, int index);
        static void PushBoundActionMethod(lua_State* L, int index);
        static void PushBoundCameraMethod(lua_State* L, int index);

        static int Lua_DispatchGlobal(lua_State* L);
        static int Lua_DispatchObjectMethod(lua_State* L);
        static int Lua_DispatchActionMethod(lua_State* L);
        static int Lua_DispatchCameraMethod(lua_State* L);

        static int Lua_New(lua_State* L);
        static int Lua_Remove(lua_State* L);
        static int Lua_SnapshotIterator(lua_State* L);

        static int Lua_ObjectIndex(lua_State* L);
        static int Lua_ObjectAddComponent(lua_State* L);
        static int Lua_ObjectRemoveComponent(lua_State* L);

        static int Lua_ComponentIndex(lua_State* L);
        static int Lua_ComponentNewIndex(lua_State* L);

        static Action* GetActionFromProxy(lua_State* L, int index);
        static void PushActionProxy(lua_State* L, Action* action);
        static int Lua_ActionBindKeyDown(lua_State* L);
        static int Lua_ActionBindKeyUp(lua_State* L);
        static int Lua_ActionBindLogic(lua_State* L);
        static int Lua_ActionIndex(lua_State* L);

        static int Lua_CameraIndex(lua_State* L);
        static int Lua_CameraNewIndex(lua_State* L);
        static int Lua_CameraBindTarget(lua_State* L);
        static int Lua_CameraUnbindTarget(lua_State* L);

        static int Lua_EventIndex(lua_State* L);
        static int Lua_EventBind(lua_State* L);

        static int Lua_After(lua_State* L);
        static int Lua_Every(lua_State* L);
    };

    class LuaGlobalFunctionBuilder
    {
    public:
        explicit LuaGlobalFunctionBuilder(const char* name)
        {
            _binding.name = name;
            _binding.allowedMask = LuaCtxAny;
        }

        LuaGlobalFunctionBuilder& AllowInLoad(bool value) { SetFlag(LuaCtxLoad, value); return *this; }
        LuaGlobalFunctionBuilder& AllowInUpdate(bool value) { SetFlag(LuaCtxUpdate, value); return *this; }
        LuaGlobalFunctionBuilder& AllowInDraw(bool value) { SetFlag(LuaCtxDraw, value); return *this; }
        LuaGlobalFunctionBuilder& AllowOutside(bool value) { SetFlag(LuaCtxOutside, value); return *this; }
        LuaGlobalFunctionBuilder& AllowedMask(int mask) { _binding.allowedMask = mask; return *this; }
        LuaGlobalFunctionBuilder& Bind(const std::function<int(lua_State*)>& callback)
        {
            _binding.callback = callback;
            return *this;
        }

        void Register() const
        {
            LuaAPI::RegisterGlobalFunction(_binding);
        }

    private:
        void SetFlag(int flag, bool enabled)
        {
            if (enabled) _binding.allowedMask |= flag;
            else _binding.allowedMask &= ~flag;
        }

        LuaCallableBinding _binding;
    };

    class LuaObjectMethodBuilder
    {
    public:
        explicit LuaObjectMethodBuilder(const char* name)
        {
            _binding.name = name;
            _binding.allowedMask = LuaCtxAny;
        }

        LuaObjectMethodBuilder& AllowInLoad(bool value) { SetFlag(LuaCtxLoad, value); return *this; }
        LuaObjectMethodBuilder& AllowInUpdate(bool value) { SetFlag(LuaCtxUpdate, value); return *this; }
        LuaObjectMethodBuilder& AllowInDraw(bool value) { SetFlag(LuaCtxDraw, value); return *this; }
        LuaObjectMethodBuilder& AllowOutside(bool value) { SetFlag(LuaCtxOutside, value); return *this; }
        LuaObjectMethodBuilder& AllowedMask(int mask) { _binding.allowedMask = mask; return *this; }
        LuaObjectMethodBuilder& Bind(const std::function<int(lua_State*)>& callback)
        {
            _binding.callback = callback;
            return *this;
        }

        void Register() const
        {
            LuaAPI::RegisterObjectMethod(_binding);
        }

    private:
        void SetFlag(int flag, bool enabled)
        {
            if (enabled) _binding.allowedMask |= flag;
            else _binding.allowedMask &= ~flag;
        }

        LuaCallableBinding _binding;
    };

    class LuaActionMethodBuilder
    {
    public:
        explicit LuaActionMethodBuilder(const char* name)
        {
            _binding.name = name;
            _binding.allowedMask = LuaCtxAny;
        }

        LuaActionMethodBuilder& AllowInLoad(bool value) { SetFlag(LuaCtxLoad, value); return *this; }
        LuaActionMethodBuilder& AllowInUpdate(bool value) { SetFlag(LuaCtxUpdate, value); return *this; }
        LuaActionMethodBuilder& AllowInDraw(bool value) { SetFlag(LuaCtxDraw, value); return *this; }
        LuaActionMethodBuilder& AllowOutside(bool value) { SetFlag(LuaCtxOutside, value); return *this; }
        LuaActionMethodBuilder& AllowedMask(int mask) { _binding.allowedMask = mask; return *this; }
        LuaActionMethodBuilder& Bind(const std::function<int(lua_State*)>& callback)
        {
            _binding.callback = callback;
            return *this;
        }

        void Register() const
        {
            LuaAPI::RegisterActionMethod(_binding);
        }

    private:
        void SetFlag(int flag, bool enabled)
        {
            if (enabled) _binding.allowedMask |= flag;
            else _binding.allowedMask &= ~flag;
        }

        LuaCallableBinding _binding;
    };

    class LuaCameraMethodBuilder
    {
    public:
        explicit LuaCameraMethodBuilder(const char* name)
        {
            _binding.name = name;
            _binding.allowedMask = LuaCtxAny;
        }

        LuaCameraMethodBuilder& AllowInLoad(bool value) { SetFlag(LuaCtxLoad, value); return *this; }
        LuaCameraMethodBuilder& AllowInUpdate(bool value) { SetFlag(LuaCtxUpdate, value); return *this; }
        LuaCameraMethodBuilder& AllowInDraw(bool value) { SetFlag(LuaCtxDraw, value); return *this; }
        LuaCameraMethodBuilder& AllowOutside(bool value) { SetFlag(LuaCtxOutside, value); return *this; }
        LuaCameraMethodBuilder& AllowedMask(int mask) { _binding.allowedMask = mask; return *this; }
        LuaCameraMethodBuilder& Bind(const std::function<int(lua_State*)>& callback)
        {
            _binding.callback = callback;
            return *this;
        }

        void Register() const
        {
            LuaAPI::RegisterCameraMethod(_binding);
        }

    private:
        void SetFlag(int flag, bool enabled)
        {
            if (enabled) _binding.allowedMask |= flag;
            else _binding.allowedMask &= ~flag;
        }

        LuaCallableBinding _binding;
    };

    class LuaConstantTableBuilder
    {
    public:
        explicit LuaConstantTableBuilder(const char* tableName)
        {
            _binding.tableName = tableName;
        }

        LuaConstantTableBuilder& Int(const char* name, long long value)
        {
            LuaConstantEntry entry;
            entry.kind = LuaConstantKind::Integer;
            entry.name = name;
            entry.integerValue = value;
            _binding.entries.push_back(entry);
            return *this;
        }

        LuaConstantTableBuilder& Number(const char* name, double value)
        {
            LuaConstantEntry entry;
            entry.kind = LuaConstantKind::Number;
            entry.name = name;
            entry.numberValue = value;
            _binding.entries.push_back(entry);
            return *this;
        }

        LuaConstantTableBuilder& Bool(const char* name, bool value)
        {
            LuaConstantEntry entry;
            entry.kind = LuaConstantKind::Boolean;
            entry.name = name;
            entry.boolValue = value;
            _binding.entries.push_back(entry);
            return *this;
        }

        LuaConstantTableBuilder& String(const char* name, const char* value)
        {
            LuaConstantEntry entry;
            entry.kind = LuaConstantKind::String;
            entry.name = name;
            entry.stringValue = value;
            _binding.entries.push_back(entry);
            return *this;
        }

        LuaConstantTableBuilder& Color(const char* name, Color value)
        {
            LuaConstantEntry entry;
            entry.kind = LuaConstantKind::Color;
            entry.name = name;
            entry.colorValue = value;
            _binding.entries.push_back(entry);
            return *this;
        }

        void Register() const
        {
            LuaAPI::RegisterConstantTable(_binding);
        }

    private:
        LuaConstantTableBinding _binding;
    };
}