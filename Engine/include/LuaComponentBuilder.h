#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include "Registry.h"
#include "LuaAPI.h"
#include "TypeEvent.h"
#include "TypeObject.h"

namespace Engine
{
    template<typename T>
    class LuaComponentBuilder;

    template<typename T>
    class LuaComponentMethodBuilder
    {
    public:
        LuaComponentMethodBuilder(LuaComponentBinding& binding, LuaComponentBuilder<T>* parent, const char* name)
            : _binding(binding), _parent(parent)
        {
            _method.name = name;
        }

        LuaComponentMethodBuilder& Contexts(int callMask)
        {
            _method.callMask = callMask;
            return *this;
        }

        LuaComponentBuilder<T>& Bind(const std::function<int(lua_State*, T*)>& fn)
        {
            _method.call = [fn](lua_State* L, Object* owner) -> int
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component) return 0;
                    return fn(L, component);
                };

            _binding.methods.push_back(_method);
            return *_parent;
        }

        LuaComponentBuilder<T>& Void(void (T::* method)())
        {
            return Bind([method](lua_State* L, T* component) -> int
                {
                    (void)L;
                    (component->*method)();
                    return 0;
                });
        }

        LuaComponentBuilder<T>& Number(void (T::* method)(float))
        {
            return Bind([method](lua_State* L, T* component) -> int
                {
                    float a = static_cast<float>(luaL_checknumber(L, 2));
                    (component->*method)(a);
                    return 0;
                });
        }

        LuaComponentBuilder<T>& Integer(void (T::* method)(int))
        {
            return Bind([method](lua_State* L, T* component) -> int
                {
                    int a = static_cast<int>(luaL_checkinteger(L, 2));
                    (component->*method)(a);
                    return 0;
                });
        }

        LuaComponentBuilder<T>& Bool(void (T::* method)(bool))
        {
            return Bind([method](lua_State* L, T* component) -> int
                {
                    bool a = lua_toboolean(L, 2) != 0;
                    (component->*method)(a);
                    return 0;
                });
        }

        LuaComponentBuilder<T>& NumberNumber(void (T::* method)(float, float))
        {
            return Bind([method](lua_State* L, T* component) -> int
                {
                    float a = static_cast<float>(luaL_checknumber(L, 2));
                    float b = static_cast<float>(luaL_checknumber(L, 3));
                    (component->*method)(a, b);
                    return 0;
                });
        }

        LuaComponentBuilder<T>& NumberNumberNumber(void (T::* method)(float, float, float))
        {
            return Bind([method](lua_State* L, T* component) -> int
                {
                    float a = static_cast<float>(luaL_checknumber(L, 2));
                    float b = static_cast<float>(luaL_checknumber(L, 3));
                    float c = static_cast<float>(luaL_checknumber(L, 4));
                    (component->*method)(a, b, c);
                    return 0;
                });
        }

    private:
        LuaComponentBinding& _binding;
        LuaComponentBuilder<T>* _parent = nullptr;
        LuaMethodBinding _method;
    };

    template<typename T>
    class LuaComponentBuilder
    {
    public:
        explicit LuaComponentBuilder(const char* luaName)
        {
            _luaName = luaName;

            _binding.luaName = luaName;
            _binding.get = [](Object* object) -> Component*
                {
                    if (!object) return nullptr;
                    return object->GetComponent<T>();
                };

            _binding.destroy = [](Object* object)
                {
                    if (!object) return;

                    if (auto* component = object->GetComponent<T>())
                        Registry::DestroyComponent(component);
                };
        }

        LuaComponentBuilder& Group(const char* name)
        {
            _binding.groupA = name;
            return *this;
        }

        LuaComponentBuilder& Group2(const char* name)
        {
            _binding.groupB = name;
            return *this;
        }

        LuaComponentBuilder& Creator(const std::function<Component* (Object*)>& creator)
        {
            _binding.create = creator;
            return *this;
        }

        LuaComponentBuilder& Destroyer(const std::function<void(Object*)>& destroyer)
        {
            _binding.destroy = destroyer;
            return *this;
        }

        LuaComponentMethodBuilder<T> Method(const char* name)
        {
            return LuaComponentMethodBuilder<T>(_binding, this, name);
        }

        LuaComponentBuilder<T>& Event(const char* name, TypeEvent* (T::* getter)())
        {
            LuaEventFieldBinding entry;
            entry.name = name;
            entry.getEvent = [getter](Object* owner) -> TypeEvent*
                {
                    T* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component) return nullptr;
                    return (component->*getter)();
                };

            _binding.events.push_back(entry);
            return *this;
        }

        template<typename Getter, typename Setter>
        LuaComponentBuilder& FieldNumber(
            const char* name,
            Getter getter,
            Setter setter,
            int readMask = LuaCtxAny,
            int writeMask = LuaCtxAny)
        {
            LuaFieldBinding field;
            field.name = name;
            field.readMask = readMask;
            field.writeMask = writeMask;

            field.push = [getter](lua_State* L, Object* owner) -> int
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component)
                    {
                        lua_pushnil(L);
                        return 1;
                    }

                    lua_pushnumber(L, static_cast<lua_Number>((component->*getter)()));
                    return 1;
                };

            field.set = [setter](lua_State* L, Object* owner, int valueIndex) -> bool
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component) return false;

                    (component->*setter)(static_cast<float>(luaL_checknumber(L, valueIndex)));
                    return true;
                };

            _binding.fields.push_back(field);
            return *this;
        }

        template<typename Getter>
        LuaComponentBuilder& FieldNumberRO(
            const char* name,
            Getter getter,
            int readMask = LuaCtxAny)
        {
            LuaFieldBinding field;
            field.name = name;
            field.readMask = readMask;
            field.writeMask = LuaCtxNone;

            field.push = [getter](lua_State* L, Object* owner) -> int
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component)
                    {
                        lua_pushnil(L);
                        return 1;
                    }

                    lua_pushnumber(L, static_cast<lua_Number>((component->*getter)()));
                    return 1;
                };

            field.set = nullptr;

            _binding.fields.push_back(field);
            return *this;
        }

        template<typename Getter, typename Setter>
        LuaComponentBuilder& FieldInteger(
            const char* name,
            Getter getter,
            Setter setter,
            int readMask = LuaCtxAny,
            int writeMask = LuaCtxAny)
        {
            LuaFieldBinding field;
            field.name = name;
            field.readMask = readMask;
            field.writeMask = writeMask;

            field.push = [getter](lua_State* L, Object* owner) -> int
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component)
                    {
                        lua_pushnil(L);
                        return 1;
                    }

                    lua_pushinteger(L, static_cast<lua_Integer>((component->*getter)()));
                    return 1;
                };

            field.set = [setter](lua_State* L, Object* owner, int valueIndex) -> bool
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component) return false;

                    (component->*setter)(static_cast<int>(luaL_checkinteger(L, valueIndex)));
                    return true;
                };

            _binding.fields.push_back(field);
            return *this;
        }

        template<typename Getter, typename Setter>
        LuaComponentBuilder& FieldBool(
            const char* name,
            Getter getter,
            Setter setter,
            int readMask = LuaCtxAny,
            int writeMask = LuaCtxAny)
        {
            LuaFieldBinding field;
            field.name = name;
            field.readMask = readMask;
            field.writeMask = writeMask;

            field.push = [getter](lua_State* L, Object* owner) -> int
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component)
                    {
                        lua_pushnil(L);
                        return 1;
                    }

                    lua_pushboolean(L, (component->*getter)() ? 1 : 0);
                    return 1;
                };

            field.set = [setter](lua_State* L, Object* owner, int valueIndex) -> bool
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component) return false;

                    (component->*setter)(lua_toboolean(L, valueIndex) != 0);
                    return true;
                };

            _binding.fields.push_back(field);
            return *this;
        }

        LuaComponentBuilder& FieldColorMember(
            const char* name,
            Color T::* member,
            int readMask = LuaCtxAny,
            int writeMask = LuaCtxAny)
        {
            LuaFieldBinding field;
            field.name = name;
            field.readMask = readMask;
            field.writeMask = writeMask;

            field.push = [member](lua_State* L, Object* owner) -> int
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component)
                    {
                        lua_pushnil(L);
                        return 1;
                    }

                    LuaAPI::PushColorTableFromRaylib(L, component->*member);
                    return 1;
                };

            field.set = [member](lua_State* L, Object* owner, int valueIndex) -> bool
                {
                    auto* component = owner ? owner->GetComponent<T>() : nullptr;
                    if (!component) return false;

                    unsigned char r, g, b, a;
                    if (!LuaAPI::ReadColorTable(L, valueIndex, r, g, b, a))
                        return false;

                    component->*member = Color{ r, g, b, a };
                    return true;
                };

            _binding.fields.push_back(field);
            return *this;
        }

        LuaComponentBuilder& FieldCustom(const LuaFieldBinding& field)
        {
            _binding.fields.push_back(field);
            return *this;
        }

        void Register() const
        {
            LuaComponentBinding binding = _binding;
            const char* luaName = _luaName;

            binding.appendSnapshot = [luaName](lua_State* L, int tableIndex, int* nextIndex)
                {
                    tableIndex = lua_absindex(L, tableIndex);

                    for (auto* object : Registry::GetAllObjects())
                    {
                        if (!object) continue;

                        T* component = object->GetComponent<T>();
                        if (!component) continue;

                        LuaAPI::PushComponentProxy(L, object, luaName);
                        lua_rawseti(L, tableIndex, (*nextIndex)++);
                    }
                };

            binding.pushField = nullptr;
            binding.setField = nullptr;

            LuaAPI::RegisterComponentBinding(binding);
        }

    private:
        const char* _luaName = nullptr;
        LuaComponentBinding _binding;
    };
}