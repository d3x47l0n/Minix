#pragma once

#include "box2d/box2d.h"

namespace Engine
{
	class PhysicsRuntime
	{
	public:
		static void Init();
		static void Stop();
		static void Update(float dt);

		static b2WorldId GetWorld();

	private:
		static b2WorldId _world;
		static bool _active;
	};
}