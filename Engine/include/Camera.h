#pragma once

#include "raylib.h"
#include "ComponentPosition.h"

namespace Engine
{
	class Camera
	{
	public:
		static void Init(int w, int h);
		static void Stop();
		static void Update(float dt);

		static void SetScreenSize(int w, int h);

		static void SetX(float x);
		static void SetY(float y);
		static void SetA(float a);
		static void SetZoom(float zoom);

		static float GetX();
		static float GetY();
		static float GetA();
		static float GetZoom();

		static void BindTarget(Position2D* target);
		static void UnbindTarget();
		static Position2D* GetTarget();

		static Camera2D GetRaylibCamera();

	private:
		static bool TargetExists();

		static float _x;
		static float _y;
		static float _a;
		static float _zoom;

		static int _w;
		static int _h;

		static Position2D* _target;
	};
}