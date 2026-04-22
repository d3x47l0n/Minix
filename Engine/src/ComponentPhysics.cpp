#include "ComponentPhysics.h"

#include "LuaComponentBuilder.h"
#include "Registry.h"
#include "PhysicsRuntime.h"

namespace Engine
{
	void RectanglePhysics2D::RegisterLuaBinding()
	{
		LuaComponentBuilder<RectanglePhysics2D>("rectanglePhysics2D")
			.Group("physics2D")
			.Group2("rectanglePhysics2D")
			.Creator([](Object* object) -> Component*
				{
					if (auto* c = object->GetComponent<RectanglePhysics2D>())
						return c;

					b2WorldId world = PhysicsRuntime::GetWorld();
					if (B2_IS_NULL(world))
					{
						TraceLog(LOG_ERROR, "RectanglePhysics2D creator: Physics world is null");
						return nullptr;
					}

					return Registry::CreateComponent<RectanglePhysics2D>(
						object,
						world,
						false,
						0.0f, 0.0f, 0.0f,
						32.0f, 32.0f,
						1.0f, 0.3f
					);
				})
			.FieldNumber("x", &RectanglePhysics2D::GetX, &RectanglePhysics2D::SetX)
			.FieldNumber("y", &RectanglePhysics2D::GetY, &RectanglePhysics2D::SetY)
			.FieldNumber("a", &RectanglePhysics2D::GetA, &RectanglePhysics2D::SetA)
			.FieldNumber("w", &RectanglePhysics2D::GetW, &RectanglePhysics2D::SetW)
			.FieldNumber("h", &RectanglePhysics2D::GetH, &RectanglePhysics2D::SetH)
			.FieldBool("anchored", &RectanglePhysics2D::GetAnchored, &RectanglePhysics2D::SetAnchored)
			.FieldBool("lockX", &RectanglePhysics2D::GetLockX, &RectanglePhysics2D::SetLockX)
			.FieldBool("lockY", &RectanglePhysics2D::GetLockY, &RectanglePhysics2D::SetLockY)
			.FieldBool("lockA", &RectanglePhysics2D::GetLockA, &RectanglePhysics2D::SetLockA)
			.Method("move").Contexts(LuaCtxUpdate).NumberNumber(&RectanglePhysics2D::Move)
			.Method("moveX").Contexts(LuaCtxUpdate).Number(&RectanglePhysics2D::MoveX)
			.Method("moveY").Contexts(LuaCtxUpdate).Number(&RectanglePhysics2D::MoveY)
			.Method("moveA").Contexts(LuaCtxUpdate).Number(&RectanglePhysics2D::MoveA)
			.Method("yeet").Contexts(LuaCtxUpdate).NumberNumber(&RectanglePhysics2D::Yeet)
			.Method("yeetX").Contexts(LuaCtxUpdate).Number(&RectanglePhysics2D::YeetX)
			.Method("yeetY").Contexts(LuaCtxUpdate).Number(&RectanglePhysics2D::YeetY)
			.Event("touched", &RectanglePhysics2D::GetTouchedEvent)
			.Register();
	}

	void CirclePhysics2D::RegisterLuaBinding()
	{
		LuaComponentBuilder<CirclePhysics2D>("circlePhysics2D")
			.Group("physics2D")
			.Group2("circlePhysics2D")
			.Creator([](Object* object) -> Component*
				{
					if (auto* c = object->GetComponent<CirclePhysics2D>())
						return c;

					b2WorldId world = PhysicsRuntime::GetWorld();
					if (B2_IS_NULL(world))
					{
						TraceLog(LOG_ERROR, "CirclePhysics2D creator: Physics world is null");
						return nullptr;
					}

					return Registry::CreateComponent<CirclePhysics2D>(
						object,
						world,
						false,
						0.0f, 0.0f,
						16.0f,
						1.0f, 0.3f
					);
				})
			.FieldNumber("x", &CirclePhysics2D::GetX, &CirclePhysics2D::SetX)
			.FieldNumber("y", &CirclePhysics2D::GetY, &CirclePhysics2D::SetY)
			.FieldNumber("a", &CirclePhysics2D::GetA, &CirclePhysics2D::SetA)
			.FieldNumber("r", &CirclePhysics2D::GetR, &CirclePhysics2D::SetR)
			.FieldBool("anchored", &CirclePhysics2D::GetAnchored, &CirclePhysics2D::SetAnchored)
			.FieldBool("lockX", &CirclePhysics2D::GetLockX, &CirclePhysics2D::SetLockX)
			.FieldBool("lockY", &CirclePhysics2D::GetLockY, &CirclePhysics2D::SetLockY)
			.FieldBool("lockA", &CirclePhysics2D::GetLockA, &CirclePhysics2D::SetLockA)
			.Method("move").Contexts(LuaCtxUpdate).NumberNumber(&CirclePhysics2D::Move)
			.Method("moveX").Contexts(LuaCtxUpdate).Number(&CirclePhysics2D::MoveX)
			.Method("moveY").Contexts(LuaCtxUpdate).Number(&CirclePhysics2D::MoveY)
			.Method("moveA").Contexts(LuaCtxUpdate).Number(&CirclePhysics2D::MoveA)
			.Method("yeet").Contexts(LuaCtxUpdate).NumberNumber(&CirclePhysics2D::Yeet)
			.Method("yeetX").Contexts(LuaCtxUpdate).Number(&CirclePhysics2D::YeetX)
			.Method("yeetY").Contexts(LuaCtxUpdate).Number(&CirclePhysics2D::YeetY)
			.Event("touched", &CirclePhysics2D::GetTouchedEvent)
			.Register();
	}
}