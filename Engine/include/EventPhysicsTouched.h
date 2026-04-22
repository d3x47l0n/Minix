#pragma once

#include "TypeEvent.h"

namespace Engine
{
	class Object;
	class Component;

	struct PhysicsTouchedPayload
	{
		Object* otherObject = nullptr;
		Component* otherComponent = nullptr;
	};

	class PhysicsTouchedEvent : public TypeEvent
	{
	public:
		explicit PhysicsTouchedEvent(const char* name = "touched")
			: TypeEvent(name)
		{
		}

		int PushPayload(lua_State* L, const void* payload) override;
	};
}