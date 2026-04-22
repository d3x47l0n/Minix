#include "InputRuntime.h"

namespace Engine
{
	std::vector<Action*> InputRuntime::_actions;
	bool InputRuntime::_acceptingActions = false;
	bool InputRuntime::_active = false;

	void InputRuntime::Init()
	{
		for (auto* action : _actions)
		{
			delete action;
		}

		_actions.clear();
		_acceptingActions = true;
		_active = true;
	}

	void InputRuntime::Stop()
	{
		for (auto* action : _actions)
		{
			delete action;
		}

		_actions.clear();
		_acceptingActions = false;
		_active = false;
	}

	void InputRuntime::Lock()
	{
		_acceptingActions = false;
	}

	void InputRuntime::Update(float dt)
	{
		if (!_active) return;

		for (auto* action : _actions)
		{
			if (action)
				action->Update(dt);
		}
	}

	bool InputRuntime::IsAcceptingActions()
	{
		return _acceptingActions;
	}

	Action* InputRuntime::CreateAction()
	{
		if (!_acceptingActions) return nullptr;

		Action* action = new Action();
		_actions.push_back(action);
		return action;
	}

	void InputRuntime::BindKeyDown(Action* action, int key)
	{
		if (!_acceptingActions) return;
		if (!action) return;

		action->BindKeyDown(key);
	}

	void InputRuntime::BindKeyUp(Action* action, int key)
	{
		if (!_acceptingActions) return;
		if (!action) return;

		action->BindKeyUp(key);
	}

	void InputRuntime::UnbindKeyDown(Action* action, int key)
	{
		if (!action) return;

		action->UnbindKeyDown(key);
	}

	void InputRuntime::UnbindKeyUp(Action* action, int key)
	{
		if (!action) return;

		action->UnbindKeyUp(key);
	}

	void InputRuntime::BindLogic(Action* action, const std::function<void(float)>& logic)
	{
		if (!_acceptingActions) return;
		if (!action) return;

		action->BindLogic(logic);
	}

	const std::vector<Action*>& InputRuntime::GetActions()
	{
		return _actions;
	}
}