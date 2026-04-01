#pragma once

#include <vector>

namespace Engine
{
    class Registry;
    class Component;

    class Object
    {
    friend class Registry;
    public:
        Object(int id);
        int GetID() const
        {
            return _id;
        };
        Object* GetParent() const
        {
            return _parent;
        };
        const std::vector<Object*>& GetChildren() const
        {
            return _children;
        };
        const std::vector<Component*>& GetComponents() const
        {
            return _components;
        };
        template<typename T>
        T* GetComponent() const;

    private:
        int _id;
        Object* _parent = nullptr;
        std::vector<Object*> _children;
        std::vector<Component*> _components;
    };
}

#include "TypeObject.tpp"