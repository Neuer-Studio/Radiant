#include <Radiant/Scene/Scene.hpp>
#include <Radiant/Scene/Entity.hpp>
#include <Radiant/Rendering/SceneRendering.hpp>

namespace Radiant
{
	/*=========== Scene ============*/

	Scene::Scene(const std::string& SceneName)
		: m_SceneName(SceneName), m_UUID()
	{

	}

	Scene::~Scene()
	{
		//TODO(Danya): Delete Entitys
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
	
	Entity* Scene::GetEntityByComponentType(ComponentType type)
	{
		for (auto e : m_Entitys)
		{
			if (e->HasComponent(type))
				return e;
		}

		RADIANT_VERIFY(false, "Scene doest have the component.");
		return nullptr;
	}

	bool Scene::ContainsEntityInScene(ComponentType type)
	{
		for (auto e : m_Entitys)
		{
			if (e->HasComponent(type))
				return true;
		}

		return false;
	}

	void Scene::UpdateScene(const Memory::Shared<SceneRendering>& rendering, SceneType type) // TODO(Danya): Update scene render 
	{
		m_ViewportWidth = rendering->m_ViewportWidth;
		m_ViewportHeight = rendering->m_ViewportHeight;

		Memory::Shared<SceneRendering> render = rendering;
		render->SetScene(this);
		for (const auto e : m_Entitys)
		{
			if (e->HasComponent(ComponentType::Mesh))
			{
				rendering->AddMeshToDrawList(e->GetComponent(ComponentType::Mesh).As<MeshComponent>()->Mesh);
			}
		}

		render->SubmitScene();
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