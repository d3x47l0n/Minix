#pragma once

#include "TypeComponent.h"
#include "TypeCoords.h"

namespace Engine
{
	class Position2D : public ComponentBase<Position2D>, public Coords2D
	{
	public:
		explicit Position2D(float x, float y, float a)
			: Coords2D(x, y, a)
		{
		}

		static void RegisterLuaBinding();
	};

	class Force2D : public ComponentBase<Force2D>, public Coords2D
	{
	public:
		explicit Force2D(float x, float y, float a)
			: Coords2D(x, y, a)
		{
		}

		static void RegisterLuaBinding();
	};
}