#pragma once

#include <vector>
#include <algorithm>

struct lua_State;

namespace Engine
{
	class Registry;
	class Object;

	class Component
	{
		friend class Registry;
	public:
		virtual ~Component() = default;

		Object* GetOwner() const
		{
			return _owner;
		}

		virtual void AddToList() = 0;
		virtual void RemoveFromList() = 0;
		virtual void CleanupLuaState(lua_State* L) { (void)L; }

	private:
		Object* _owner = nullptr;
	};

	template<typename T>
	class ComponentBase : public Component
	{
		friend class Registry;
	public:
		static const std::vector<T*>& GetList()
		{
			return _list;
		}

		void AddToList() override
		{
			_list.push_back(static_cast<T*>(this));
		}

		void RemoveFromList() override
		{
			auto& list = _list;
			auto it = std::find(list.begin(), list.end(), static_cast<T*>(this));
			if (it != list.end())
				list.erase(it);
		}

	protected:
		inline static std::vector<T*> _list;
	};
}