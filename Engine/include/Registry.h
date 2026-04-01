#pragma once

#include <vector>
#include <utility>

#include "TypeObject.h"
#include "TypeComponent.h"

namespace Engine
{
    class Registry
    {
    public:
        Registry();
        ~Registry();

        // object
        // === base
        Object* CreateObject();
        void DestroyObject(Object* object);
        const std::vector<Object*>& GetAllObjects() const;
        // === hierarchy
        void AttachChild(Object* parent, Object* child);
        void DetachChild(Object* parent, Object* child);

        // component
        template<typename T, typename... Args>
        T* CreateComponent(Object* owner, Args&&... args);

        void DestroyComponent(Component* component);

    private:
        std::vector<Object*> _objects;
        std::vector<Component*> _components;
        int _idCounter = 0;
    };
}

#include "Registry.tpp"