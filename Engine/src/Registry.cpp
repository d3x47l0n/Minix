#include "Registry.h"

namespace Engine
{
	Object::Object(int id)
		: _id(id)
	{

	}

	Registry::Registry()
	{

	}

	Registry::~Registry()
	{
		for (auto* object : _objects)
		{
			delete object;
		}
		_objects.clear();
		for (auto* component : _components)
		{
			delete component;
		}
		_components.clear();
	}

	Object* Registry::CreateObject()
	{
		Object* object = new Object(_idCounter++);
		_objects.push_back(object);
		return object;
	}

	void Registry::DestroyObject(Object* object)
	{
		if (!object) return;

		const std::vector<Object*> childrenCopy = object->GetChildren();

		for (auto* child : childrenCopy)
		{
			DestroyObject(child);
		}

		DetachChild(object->GetParent(), object);

		const std::vector<Component*> componentsCopy = object->GetComponents();

		for (auto* component : componentsCopy)
		{
			DestroyComponent(component);
		}

		auto it = std::find(_objects.begin(), _objects.end(), object);
		if (it != _objects.end())
		{
			_objects.erase(it);
		}

		delete object;
	}

	const std::vector<Object*>& Registry::GetAllObjects() const
	{
		return _objects;
	}

	void Registry::AttachChild(Object* parent, Object* child)
	{
		if (!parent || !child) return;
		if (child->_parent != nullptr) return;

		parent->_children.push_back(child);
		child->_parent = parent;
	}

	void Registry::DetachChild(Object* parent, Object* child)
	{
		if (!parent || !child) return;
		if (child->_parent != parent) return;

		auto& children = parent->_children;

		auto it = std::find(children.begin(), children.end(), child);

		if (it != children.end())
		{
			children.erase(it);
			child->_parent = nullptr;
		}
	}

	void Registry::DestroyComponent(Component* component)
	{
		if (!component) return;

		Object* owner = component->_owner;

		if (owner)
		{
			auto& componentsOwner = owner->_components;

			auto it = std::find(componentsOwner.begin(), componentsOwner.end(), component);
			if (it != componentsOwner.end())
				componentsOwner.erase(it);
		}

		auto& componentsRegistry = _components;

		auto it = std::find(componentsRegistry.begin(), componentsRegistry.end(), component);
		if (it != componentsRegistry.end())
			componentsRegistry.erase(it);

		component->RemoveFromList();

		delete component;
	}

}