#include <Radiant/Scene/Entity.hpp>

namespace Radiant
{
	Entity::Entity(const std::string& name)
		: m_Name(name), m_UUID()
	{

	}

	Entity::~Entity()
	{
		for (auto comp : m_Components)
			delete comp.second;
	}

	void Entity::AddComponent(Component* component)
	{
		//TODO(Danya): Add a RADIANT_VERIFY
		m_Components[component->GetType()] = component;
	}

	void Entity::RemoveComponent(Component* component)
	{
		auto it = m_Components.find(component->GetType());
		if (it != m_Components.end())
		{
			delete it->second; 
			m_Components.erase(it); 
		}
	}
}