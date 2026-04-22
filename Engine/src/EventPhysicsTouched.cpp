#include "EventPhysicsTouched.h"

#include "LuaAPI.h"

namespace Engine
{
    int PhysicsTouchedEvent::PushPayload(lua_State* L, const void* payload)
    {
        const PhysicsTouchedPayload* p = static_cast<const PhysicsTouchedPayload*>(payload);
        if (!p || !p->otherObject)
            return 0;

        LuaAPI::PushObjectProxy(L, p->otherObject);
        return 1;
    }
}