#include "PhysicsRuntime.h"

#include "TypeObject.h"
#include "ComponentPhysics.h"
#include "ComponentPosition.h"

namespace Engine
{
	b2WorldId PhysicsRuntime::_world{};
	bool PhysicsRuntime::_active = false;

	void PhysicsRuntime::Init()
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = b2Vec2{ 0.0f, -9.81f };
		_world = b2CreateWorld(&worldDef);
		_active = true;

		TraceLog(LOG_INFO, "PhysicsRuntime initialized");
	}

	void PhysicsRuntime::Stop()
	{
		if (_active)
		{
			b2DestroyWorld(_world);
			_world = b2WorldId{};
			_active = false;
		}
	}

	b2WorldId PhysicsRuntime::GetWorld()
	{
		return _world;
	}

	void PhysicsRuntime::Update(float dt)
	{
		if (!_active) return;

		for (auto* force : Force2D::GetList())
		{
			if (!force) continue;

			Object* owner = force->GetOwner();
			if (!owner) continue;

			auto* phys = owner->GetComponent<Physics2D>();
			if (!phys) continue;
			if (!phys->HasBody()) continue;

			phys->ApplyForceFromComponent(force->GetX(), force->GetY(), force->GetA());
		}

		b2World_Step(_world, dt, 4);

		b2ContactEvents contactEvents = b2World_GetContactEvents(_world);

		for (int i = 0; i < contactEvents.beginCount; ++i)
		{
			const auto& ev = contactEvents.beginEvents[i];

			b2BodyId bodyA = b2Shape_GetBody(ev.shapeIdA);
			b2BodyId bodyB = b2Shape_GetBody(ev.shapeIdB);

			auto* physA = static_cast<Physics2D*>(b2Body_GetUserData(bodyA));
			auto* physB = static_cast<Physics2D*>(b2Body_GetUserData(bodyB));

			if (!physA || !physB) continue;
			if (!physA->HasBody() || !physB->HasBody()) continue;

			Object* ownerA = physA->GetOwner();
			Object* ownerB = physB->GetOwner();

			if (!ownerA || !ownerB) continue;

			PhysicsTouchedPayload payloadA;
			payloadA.otherObject = ownerB;
			payloadA.otherComponent = physB;

			PhysicsTouchedPayload payloadB;
			payloadB.otherObject = ownerA;
			payloadB.otherComponent = physA;

			if (TypeEvent* evA = physA->GetTouchedEvent())
			{
				Messager::CallEvent(*evA, &payloadA);
			}

			if (TypeEvent* evB = physB->GetTouchedEvent())
			{
				Messager::CallEvent(*evB, &payloadB);
			}

		}

		for (auto* phys : Physics2D::GetList())
		{
			if (!phys) continue;
			if (!phys->HasBody()) continue;

			phys->ApplyLocks();

			Object* owner = phys->GetOwner();
			if (!owner) continue;

			auto* pos = owner->GetComponent<Position2D>();
			if (!pos) continue;

			pos->SetXYA(phys->GetXYA());
		}
	}
}