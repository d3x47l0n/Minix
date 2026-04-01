#pragma once

namespace Engine
{
	template<typename T, typename... Args>
    T* Registry::CreateComponent(Object* owner, Args&&... args)
    {
        if (!owner) return nullptr;

        T* component = new T(std::forward<Args>(args)...);

        component->_owner = owner;

        _components.push_back(component);
        owner->_components.push_back(component);
        T::_list.push_back(component);

        return component;
    }


}