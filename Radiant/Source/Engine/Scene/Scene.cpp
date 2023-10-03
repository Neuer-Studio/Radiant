#include <Radiant/Scene/Scene.hpp>
#include <Radiant/Scene/Entity.hpp>

namespace Radiant
{
	/*=========== Scene ============*/

	Scene::Scene(const std::string& SceneName)
		: m_SceneName(SceneName), m_UUID()
	{

	}

	void Scene::AddEntity(const Entity* entity)
	{
		m_Entitys.push_back((Entity*)entity);
	}

	Entity* Scene::CreateEntity(const std::string& name)
	{
		return new Entity(name);
	}

	Entity* Scene::GetEntityByUUID(uint64_t uuid)
	{
		for (auto e : m_Entitys)
		{
			if (e->m_UUID == uuid)
				return e;
		}
		return nullptr;
	}

	/*======================== SceneManager ======================*/

	SceneManager::SceneManager(const std::string& name)
		: m_ManagerName(name)
	{

	}

	Memory::Ref<Scene> SceneManager::Create(const std::string& SceneName)
	{
		return Memory::Ref<Scene>::Create(SceneName);
	}
}