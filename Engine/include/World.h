#pragma once

#include "Registry.h"

namespace Engine
{
    class World
    {
    public:
        Registry registry;
        World();
        void Update();
    };
}
