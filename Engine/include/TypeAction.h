#pragma once

#include <vector>
#include <functional>
#include <algorithm>

#include "raylib.h"

namespace Engine
{
	class Action
	{
	public:
		explicit Action()
		{

		}

		void BindKeyDown(int key)
		{
			_keyDown.push_back(key);
		}

		void BindKeyUp(int key)
		{
			_keyUp.push_back(key);
		}

		void UnbindKeyDown(int key)
		{
			auto it = std::remove(_keyDown.begin(), _keyDown.end(), key);
			_keyDown.erase(it, _keyDown.end());
		}

		void UnbindKeyUp(int key)
		{
			auto it = std::remove(_keyUp.begin(), _keyUp.end(), key);
			_keyUp.erase(it, _keyUp.end());
		}

		void BindLogic(const std::function<void(float)>& logic)
		{
			_logic.push_back(logic);
		}

		void Update(float dt)
		{
			bool active = false;

			for (auto key : _keyDown)
			{
				if (IsKeyDown(key))
				{
					active = true;
					break;
				}
			}

			if (!active)
			{
				for (auto key : _keyUp)
				{
					if (IsKeyReleased(key))
					{
						active = true;
						break;
					}
				}
			}

			if (!active) return;

			for (auto& logic : _logic)
			{
				if (logic)
					logic(dt);
			}
		}

	private:
		std::vector<int> _keyDown;
		std::vector<int> _keyUp;
		std::vector<std::function<void(float)>> _logic;
	};
}