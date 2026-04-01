#pragma once

namespace Engine
{
    template<typename T>
    T* Object::GetComponent() const
    {
        for (Component* c : _components)
        {
            if (T* result = dynamic_cast<T*>(c))
            {
                return result;
            }
        }

        return nullptr;
    }
}