#pragma once 

#include <Radiant/Scene/Component.hpp>
#include <Radiant/Scene/Scene.hpp>

namespace Radiant
{
	class PanelOutliner;
	class Entity
	{
	public:
		Entity(const std::string& name = "Unnamed Entity");
		~Entity();


		const std::string& GetName() const { return m_Name; }

		void AddComponent(const Memory::Shared<Component>& component);
		void RemoveComponent(ComponentType type);
		bool HasComponent(ComponentType type);
		Memory::Shared<Component> GetComponent(ComponentType type) { return m_Components[type]; }

		//template <typename T>
		//Memory::Shared<T> GetComponent()
		//{
		//	for (auto& component : m_Components)
		//	{
		//		if (dynamic_cast<T*>(component.second.Ptr()))
		//			return component.second;
		//	}

		//	RADIANT_VERIFY(false, "The list doesn't contain this type of component")
		//	return nullptr;
		//}

		//template <typename T>
		//bool HasComponent()
		//{
		//	for (auto& component : m_Components)
		//	{
		//		if (dynamic_cast<T*>(component.second.Ptr()))
		//			return true;
		//	}

		//	return false;
		//}

		//template <typename T>
		//void RemoveComponent()
		//{
		//	//TODO(Danya): Iml.
		//}
	private:
		Scene* m_OwnerScene;
		std::unordered_map<ComponentType, Memory::Shared<Component>> m_Components; //NOTE(Danya): Entity can only have a unique component type
		Utils::UUID m_UUID;
		std::string m_Name;

		friend Scene;
		friend PanelOutliner;
	};
}