#pragma once

#include "Registry.h"

namespace Engine
{
    class World
    {
    public:
        World();
        void Update();
    private:
        Registry _registry;
    };
}
