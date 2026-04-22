#pragma once

#include "raylib.h"
#include "box2d/box2d.h"

namespace Engine
{
	struct Transform2D
	{
		float x = 0.0f;
		float y = 0.0f;
		float a = 0.0f;
	};

	class Coords
	{
	public:
		virtual ~Coords() = default;

		static void SetScale(float s) { _s = s; }
		static float GetScale() { return _s; }

	protected:
		inline static float _s = 1.0f;
	};

	class Coords2D : public Coords
	{
	public:
		explicit Coords2D()
		{
			_x = 0.0f;
			_y = 0.0f;
			_a = 0.0f;
		};

		explicit Coords2D(float x, float y)
		{
			_x = x;
			_y = y;
			_a = 0.0f;
		};

		explicit Coords2D(float x, float y, float a)
		{
			_x = x;
			_y = y;
			_a = a;
		};

		virtual void SetX(float x) { _x = x; };
		virtual void SetY(float y) { _y = y; };
		virtual void SetA(float a) { _a = a; };
		virtual void SetXY(float x, float y)
		{
			_x = x;
			_y = y;
		};
		virtual void SetXYA(float x, float y, float a)
		{
			_x = x;
			_y = y;
			_a = a;
		};
		virtual void SetXYA(Transform2D t)
		{
			_x = t.x;
			_y = t.y;
			_a = t.a;
		};

		void SetB2DX(float x)
		{
			_x = x;
		};

		void SetB2DY(float y)
		{
			_y = y;
		};

		void SetB2DA(b2Rot a)
		{
			_a = b2Rot_GetAngle(a) * (180.0f / PI) * (-1.0f);
		};

		void SetB2DXY(b2Vec2 vec)
		{
			_x = vec.x;
			_y = vec.y;
		};

		void SetB2DXYA(b2Vec2 vec, b2Rot a)
		{
			_x = vec.x;
			_y = vec.y;
			_a = b2Rot_GetAngle(a) * (180.0f / PI) * (-1.0f);
		};

		void SetRayX(float x)
		{
			_x = x / _s;
		};

		void SetRayY(float y)
		{
			_y = y * (-1.0f) / _s;
		};

		void SetRayA(float a)
		{
			_a = a;
		};

		void SetRayXY(Vector2 vec)
		{
			_x = vec.x / _s;
			_y = vec.y * (-1.0f) / _s;
		};

		void SetRayXYA(Vector2 vec, float a)
		{
			_x = vec.x / _s;
			_y = vec.y * (-1.0f) / _s;
			_a = a;
		};

		virtual float GetX() { return _x; };
		virtual float GetY() { return _y; };
		virtual float GetA() { return _a; };
		virtual Transform2D GetXYA() { return Transform2D{ _x, _y, _a }; };

		float GetB2DX() { return _x; };
		float GetB2DY() { return _y; };
		b2Vec2 GetB2DXY() { return b2Vec2{ _x, _y }; };
		b2Rot GetB2DA() { return b2MakeRot(_a * (PI / 180.0f) * (-1.0f)); };

		float GetRayX() { return _x * _s; };
		float GetRayY() { return _y * _s * (-1.0f); };
		float GetRayA() { return _a; };

	protected:
		float _x;
		float _y;
		float _a;
	};
}