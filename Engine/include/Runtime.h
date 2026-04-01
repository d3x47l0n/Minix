#pragma once

#include <memory>
#include <functional>

namespace Engine
{
	class Runtime
	{
	public:
		static void Init();
		static void Reset();
		static void Start();
		static void Stop();
		static void Update(float dt, const std::function<void(float)>& func);
		static int  GetTPS();
		static void SetTargetTPS(int TPS);
		static void DrawTPS(int x, int y);

	private:
		static float _accTick;
		static float _step;
		static float _accCount;
		static int   _count;
		static int   _TPS;
		static bool  _active;
	};
}
