#pragma once

#include "raylib.h"
#include "TypeComponent.h"

namespace Engine
{
	class Position2D : public ComponentBase<Position2D>
	{
	public:
		float x{0.0f};
		float y{0.0f};
		float a{0.0f};
		explicit Position2D(float x_, float y_, float a_) : x(x_), y(y_), a(a_) {}
	};
}