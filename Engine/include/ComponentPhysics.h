#pragma once

#include "box2d/box2d.h"
#include "Messager.h"
#include "EventPhysicsTouched.h"
#include "TypeComponent.h"
#include "TypeCoords.h"
#include "TypeEvent.h"

namespace Engine
{
	class Physics2D : public ComponentBase<Physics2D>, public Coords2D
	{
	public:
		b2BodyId id{};

		explicit Physics2D(b2WorldId world, bool anchored, float x_, float y_, float a_, float density, float friction)
			: Coords2D(x_, y_, a_)
		{
			_world = world;
			_anchored = anchored;
			_density = density;
			_friction = friction;
			_touched.SetOwner(this);
			id = b2BodyId{};
		}

		virtual ~Physics2D()
		{
			DestroyBody();
		}

		void CleanupLuaState(lua_State* L) override
		{
			Messager::Clear(_touched, L);
		}

		bool HasBody() const
		{
			return !B2_IS_NULL(id);
		}

		void DestroyBody()
		{
			if (B2_IS_NULL(id))
				return;

			if (!B2_IS_NULL(_world))
			{
				b2Body_SetUserData(id, nullptr);
				b2DestroyBody(id);
			}

			id = b2BodyId{};
		}

		void SyncToBody()
		{
			if (B2_IS_NULL(id)) return;
			b2Body_SetTransform(this->id, GetB2DXY(), GetB2DA());
		}

		void SyncFromBody()
		{
			if (B2_IS_NULL(id)) return;
			SetB2DXYA(b2Body_GetPosition(this->id), b2Body_GetRotation(this->id));
		}

		void SetX(float x) override
		{
			Coords2D::SetX(x);
			SyncToBody();
			if (_lockX) _lockXValue = x;
		}

		void SetY(float y) override
		{
			Coords2D::SetY(y);
			SyncToBody();
			if (_lockY) _lockYValue = y;
		}

		void SetA(float a) override
		{
			Coords2D::SetA(a);
			SyncToBody();
		}

		void SetXY(float x, float y) override
		{
			Coords2D::SetXY(x, y);
			SyncToBody();
			if (_lockX) _lockXValue = x;
			if (_lockY) _lockYValue = y;
		}

		void SetXYA(float x, float y, float a) override
		{
			Coords2D::SetXYA(x, y, a);
			SyncToBody();
			if (_lockX) _lockXValue = x;
			if (_lockY) _lockYValue = y;
		}

		void SetXYA(Transform2D t) override
		{
			Coords2D::SetXYA(t);
			SyncToBody();
			if (_lockX) _lockXValue = t.x;
			if (_lockY) _lockYValue = t.y;
		}

		float GetX() override
		{
			SyncFromBody();
			return Coords2D::GetX();
		}

		float GetY() override
		{
			SyncFromBody();
			return Coords2D::GetY();
		}

		float GetA() override
		{
			SyncFromBody();
			return Coords2D::GetA();
		}

		Transform2D GetXYA() override
		{
			SyncFromBody();
			return Coords2D::GetXYA();
		}

		TypeEvent* GetTouchedEvent()
		{
			return &_touched;
		}

		void SetAnchored(bool anchored)
		{
			_anchored = anchored;

			if (B2_IS_NULL(id))
				return;

			if (anchored)
				b2Body_SetType(this->id, b2_staticBody);
			else
				b2Body_SetType(this->id, b2_dynamicBody);
		}

		bool GetAnchored()
		{
			return _anchored;
		}

		void Move(float x, float y)
		{
			if (B2_IS_NULL(id)) return;
			b2Body_ApplyForceToCenter(this->id, b2Vec2{ x, y }, true);
		}

		void MoveX(float x)
		{
			Move(x, 0.0f);
		}

		void MoveY(float y)
		{
			Move(0.0f, y);
		}

		void MoveA(float a)
		{
			if (B2_IS_NULL(id)) return;
			float torque = a * (PI / 180.0f);
			b2Body_ApplyTorque(this->id, torque, true);
		}

		void Yeet(float x, float y)
		{
			if (B2_IS_NULL(id)) return;
			b2Body_ApplyLinearImpulseToCenter(this->id, b2Vec2{ x, y }, true);
		}

		void YeetX(float x)
		{
			Yeet(x, 0.0f);
		}

		void YeetY(float y)
		{
			Yeet(0.0f, y);
		}

		void SetLockX(bool b)
		{
			SyncFromBody();
			_lockX = b;
			_lockXValue = Coords2D::GetX();
		}

		void SetLockY(bool b)
		{
			SyncFromBody();
			_lockY = b;
			_lockYValue = Coords2D::GetY();
		}

		void SetLockA(bool b)
		{
			SyncFromBody();
			_lockA = b;
			if (!B2_IS_NULL(id))
				b2Body_SetFixedRotation(this->id, b);
		}

		bool GetLockX() { return _lockX; }
		bool GetLockY() { return _lockY; }
		bool GetLockA() { return _lockA; }

		void ApplyForceFromComponent(float x, float y, float a)
		{
			if (x != 0.0f || y != 0.0f)
				Move(x, y);

			if (a != 0.0f)
				MoveA(a);
		}

		void ApplyLocks()
		{
			if (B2_IS_NULL(id)) return;
			if (!_lockX && !_lockY) return;

			b2Vec2 pos = b2Body_GetPosition(this->id);
			b2Vec2 vel = b2Body_GetLinearVelocity(this->id);

			if (_lockX)
			{
				pos.x = _lockXValue;
				vel.x = 0.0f;
			}

			if (_lockY)
			{
				pos.y = _lockYValue;
				vel.y = 0.0f;
			}

			b2Body_SetLinearVelocity(this->id, vel);
			b2Body_SetTransform(this->id, pos, b2Body_GetRotation(this->id));
		}

	protected:
		virtual void CreateShape() = 0;

		void RebuildBody()
		{
			Transform2D t = GetXYA();

			b2Vec2 linearVelocity{ 0.0f, 0.0f };
			float angularVelocity = 0.0f;

			if (!B2_IS_NULL(id))
			{
				linearVelocity = b2Body_GetLinearVelocity(this->id);
				angularVelocity = b2Body_GetAngularVelocity(this->id);
				DestroyBody();
			}

			b2BodyDef body = b2DefaultBodyDef();
			if (!_anchored) body.type = b2_dynamicBody;
			body.position = b2Vec2{ t.x, t.y };
			body.rotation = b2MakeRot(t.a * (PI / 180.0f) * (-1.0f));

			this->id = b2CreateBody(_world, &body);

			CreateShape();

			b2Body_SetLinearVelocity(this->id, linearVelocity);
			b2Body_SetAngularVelocity(this->id, angularVelocity);

			if (_lockA)
				b2Body_SetFixedRotation(this->id, true);

			b2Body_SetUserData(this->id, this);
		}

	protected:
		b2WorldId _world{};
		bool _anchored{ false };
		float _density{ 1.0f };
		float _friction{ 0.3f };

		PhysicsTouchedEvent _touched;

		bool _lockX{ false };
		bool _lockY{ false };
		bool _lockA{ false };

		float _lockXValue{ 0.0f };
		float _lockYValue{ 0.0f };
	};

	class RectanglePhysics2D : public Physics2D
	{
	public:
		explicit RectanglePhysics2D(b2WorldId physics, bool anchored, float x, float y, float a, float w, float h, float density, float friction)
			: Physics2D(physics, anchored, x, y, a, density, friction), _w(w), _h(h)
		{
			b2BodyDef body = b2DefaultBodyDef();
			if (!anchored) body.type = b2_dynamicBody;
			body.position = GetB2DXY();
			body.rotation = GetB2DA();

			this->id = b2CreateBody(physics, &body);
			CreateShape();
			b2Body_SetUserData(this->id, this);
		}

		float GetW() { return _w; }
		float GetH() { return _h; }

		void SetW(float w)
		{
			_w = w;
			RebuildBody();
		}

		void SetH(float h)
		{
			_h = h;
			RebuildBody();
		}

		void SetWH(float w, float h)
		{
			_w = w;
			_h = h;
			RebuildBody();
		}

		static void RegisterLuaBinding();

	protected:
		void CreateShape() override
		{
			b2ShapeDef shape = b2DefaultShapeDef();
			shape.density = _density;
			shape.material.friction = _friction;
			shape.enableContactEvents = true;

			b2Polygon polygon = b2MakeBox(_w / 2.0f, _h / 2.0f);
			b2CreatePolygonShape(this->id, &shape, &polygon);
		}

	private:
		float _w{ 1.0f };
		float _h{ 1.0f };
	};

	class CirclePhysics2D : public Physics2D
	{
	public:
		explicit CirclePhysics2D(b2WorldId physics, bool anchored, float x, float y, float r, float density, float friction)
			: Physics2D(physics, anchored, x, y, 0.0f, density, friction), _r(r)
		{
			b2BodyDef body = b2DefaultBodyDef();
			if (!anchored) body.type = b2_dynamicBody;
			body.position = GetB2DXY();

			this->id = b2CreateBody(physics, &body);
			CreateShape();
			b2Body_SetUserData(this->id, this);
		}

		void SetR(float r)
		{
			this->_r = r;
			RebuildBody();
		}

		float GetR()
		{
			return this->_r;
		}

		static void RegisterLuaBinding();

	protected:
		void CreateShape() override
		{
			b2ShapeDef shape = b2DefaultShapeDef();
			shape.density = _density;
			shape.material.friction = _friction;
			shape.enableContactEvents = true;

			b2Circle circle;
			circle.center.x = 0.0f;
			circle.center.y = 0.0f;
			circle.radius = _r;
			b2CreateCircleShape(this->id, &shape, &circle);
		}

	private:
		float _r{ 0.5f };
	};
}