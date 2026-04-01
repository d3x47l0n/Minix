#pragma once

#include <vector>
#include <algorithm>

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
		};
		virtual void RemoveFromList() = 0;
	private:
		Object* _owner = nullptr;
	};

	template<typename T>
	class ComponentBase : public Component
	{
	public:
		static const std::vector<T*>& GetList()
		{
			return _list;
		};
		void RemoveFromList() override
		{
			auto& list = T::_list;
			list.erase(std::find(list.begin(), list.end(), static_cast<T*>(this)));
		};
	protected:
		inline static std::vector<T*> _list;
	};
}