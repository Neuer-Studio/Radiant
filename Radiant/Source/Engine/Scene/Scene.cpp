#include <Radiant/Scene/Scene.hpp>
#include <Radiant/Scene/Entity.hpp>

namespace Radiant
{
	/*=========== Scene ============*/

	Scene::Scene(const std::string& SceneName)
		: m_SceneName(SceneName), m_UUID()
	{

	}

	Scene::~Scene()
	{
		//Note(Danya): Delete Entitys
	}

	void Scene::AddEntity(const Entity* entity)
	{
		m_Entitys.push_back((Entity*)entity);
	}

	Entity* Scene::CreateEntity(const std::string& name) //NOTE(Danya): Is it right way to add a new entity to the pool?
	{
		Entity* e = new Entity(name);
		e->m_OwnerScene = this;
		m_Entitys.push_back(e);

		e->AddComponent(CreateNewComponent<TransformComponent>()); //NOTE(Danya): Every entity must have a TransformComponent

		return e;
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

	void Scene::UpdateScene(SceneType type)
	{
		for (const auto e : m_Entitys)
		{
			if (e->HasComponent(ComponentType::Mesh))
			{
				auto mesh = e->GetComponent(ComponentType::Mesh).As<MeshComponent>()->Mesh;
				//mesh->GetPipeline()->Bind();
				mesh->Update();
			}
		}
	}

	/*======================== SceneManager ======================*/

	SceneManager::SceneManager(const std::string& name)
		: m_ManagerName(name)
	{

	}

	Memory::Shared<Scene> SceneManager::GetSceneByName(const std::string& name)
	{
		for (auto s : m_ScenePool)
		{
			if (s->m_SceneName == name)
				return s;
		}
		return nullptr;
	}

	Memory::Shared<Scene> SceneManager::Create(const std::string& SceneName)
	{
		auto NewScene = Memory::Shared<Scene>::Create(SceneName);
		m_ScenePool.push_back(NewScene);
		return NewScene; 
	}
}