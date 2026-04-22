#pragma once

#include <vector>
#include <functional>

#include "TypeAction.h"

namespace Engine
{
	class InputRuntime
	{
	public:
		static void Init();
		static void Stop();
		static void Lock();
		static void Update(float dt);

		static bool IsAcceptingActions();

		static Action* CreateAction();

		static void BindKeyDown(Action* action, int key);
		static void BindKeyUp(Action* action, int key);
		static void UnbindKeyDown(Action* action, int key);
		static void UnbindKeyUp(Action* action, int key);
		static void BindLogic(Action* action, const std::function<void(float)>& logic);

		static const std::vector<Action*>& GetActions();

	private:
		static std::vector<Action*> _actions;
		static bool _acceptingActions;
		static bool _active;
	};
}