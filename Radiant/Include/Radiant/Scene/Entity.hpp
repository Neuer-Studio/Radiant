#pragma once 

#include <Radiant/Scene/Component.hpp>
#include <Radiant/Scene/Scene.hpp>

namespace Radiant
{
	class Entity
	{
	public:
		Entity();
		~Entity();
		void AddComponent(Component* component);
		void RemoveComponent(Component* component);
		Component* GetComponent(ComponentType type) { return m_Components[type]; }
	private:
		Memory::Ref<Scene> m_Scene;
		std::unordered_map<ComponentType, Component*> m_Components; //NOTE(Danya): Entity can only have a unique component type
	};
}