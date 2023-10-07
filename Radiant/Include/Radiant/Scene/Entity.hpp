#pragma once 

#include <Radiant/Scene/Component.hpp>
#include <Radiant/Scene/Scene.hpp>

namespace Radiant
{
	class Entity
	{
	public:
		Entity(const std::string& name);
		~Entity();
		void AddComponent(Memory::Shared<Component> component);
		void RemoveComponent(Memory::Shared<Component> component);
		bool HasComponent(ComponentType type);
		template <typename T>
		Memory::Shared<T> GetComponent() { return m_Components[T::StaticGetType()]; }
	private:
		Scene* m_OwnerScene;
		std::unordered_map<ComponentType, Memory::Shared<Component>> m_Components; //NOTE(Danya): Entity can only have a unique component type
		Utils::UUID m_UUID;
		std::string m_Name;

		friend Scene;
	};
}