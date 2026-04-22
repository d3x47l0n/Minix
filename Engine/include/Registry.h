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
        static void Init();
        static void Stop();

        static Object* CreateObject();
        static void DestroyObject(Object* object);
        static const std::vector<Object*>& GetAllObjects();

        static void AttachChild(Object* parent, Object* child);
        static void DetachChild(Object* parent, Object* child);

        template<typename T, typename... Args>
        static T* CreateComponent(Object* owner, Args&&... args);

        static void DestroyComponent(Component* component);

    private:
        inline static std::vector<Object*> _objects;
        inline static std::vector<Component*> _components;
        inline static int _idCounter = 0;
    };
}

#include "Registry.tpp"
