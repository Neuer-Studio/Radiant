#include <Radiant/Scene/Entity.hpp>

namespace Radiant
{
	Entity::Entity(const std::string& name)
		: m_OwnerScene(nullptr), m_Name(name), m_UUID()
	{

	}

	Entity::~Entity()
	{
	
	}

	void Entity::AddComponent(const Memory::Shared<Component>& component)
	{
		//TODO(Danya): Add a RADIANT_VERIFY
		m_Components[component->GetType()] = component;
	}

	void Entity::RemoveComponent(const Memory::Shared<Component>& component)
	{
		auto it = m_Components.find(component->GetType());
		if (it != m_Components.end())
		{
			delete it->second.Ptr();
			m_Components.erase(it); 
		}
	}

	bool Entity::HasComponent(ComponentType type)
	{
		auto it = m_Components.find(type);
		return it != m_Components.end();
	}

}