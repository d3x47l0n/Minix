#include "ComponentPosition.h"

#include "LuaComponentBuilder.h"
#include "Registry.h"

namespace Engine
{
	void Position2D::RegisterLuaBinding()
	{
		LuaComponentBuilder<Position2D>("position2D")
			.Group("position2D")
			.Creator([](Object* object) -> Component*
				{
					if (auto* c = object->GetComponent<Position2D>())
						return c;

					return Registry::CreateComponent<Position2D>(
						object,
						0.0f, 0.0f, 0.0f
					);
				})
			.FieldNumber("x", &Position2D::GetX, &Position2D::SetX)
			.FieldNumber("y", &Position2D::GetY, &Position2D::SetY)
			.FieldNumber("a", &Position2D::GetA, &Position2D::SetA)
			.Register();
	}

	void Force2D::RegisterLuaBinding()
	{
		LuaComponentBuilder<Force2D>("force2D")
			.Group("force2D")
			.Creator([](Object* object) -> Component*
				{
					if (auto* c = object->GetComponent<Force2D>())
						return c;

					return Registry::CreateComponent<Force2D>(
						object,
						0.0f, 0.0f, 0.0f
					);
				})
			.FieldNumber("x", &Force2D::GetX, &Force2D::SetX)
			.FieldNumber("y", &Force2D::GetY, &Force2D::SetY)
			.FieldNumber("a", &Force2D::GetA, &Force2D::SetA)
			.Register();
	}
}