#include <Radiant/Scene/Scene.hpp>
#include <Radiant/Scene/Entity.hpp>
#include <Radiant/Rendering/SceneRendering.hpp>

#include <glm/glm.hpp>

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

	void Scene::UpdateScene(Timestep ts, const Memory::Shared<SceneRendering>& rendering, SceneType type) // TODO(Danya): Update scene render 
	{
		m_SceneRendering = rendering;

		m_ViewportWidth = rendering->m_ViewportWidth;
		m_ViewportHeight = rendering->m_ViewportHeight;

		Camera* camera = &Camera();

		Memory::Shared<SceneRendering> render = rendering;
		render->SetScene(this);
		
		for (const auto e : m_Entitys)
		{
			if (e->HasComponent(ComponentType::Mesh))
			{
				const auto mesh = e->GetComponent(ComponentType::Mesh).As<MeshComponent>()->Mesh;
				if (!mesh) continue;
				const auto& material = e->GetComponent(ComponentType::Material).As<MaterialComponent>()->Material;
				rendering->AddMeshToDrawList
				({
						mesh,
						e->GetComponent(ComponentType::Transform).As<TransformComponent>()->GetTransform(),
						material
				});
			}

			if (e->HasComponent(ComponentType::SkyBox))
			{
				const auto cube = e->GetComponent(ComponentType::SkyBox).As<SkyBoxComponent>()->Environment;
				if (!cube) continue;
				rendering->SetEnvRadiance(cube->RadianceMap);
				rendering->SetEnvIrradiance(cube->IrradianceMap);
			}

			if (e->HasComponent(ComponentType::Camera))
			{
				camera = &e->GetComponent(ComponentType::Camera).As<CameraComponent>()->Camera;
			}
		}
		
		camera->SetViewportSize(m_ViewportWidth, m_ViewportHeight);
		camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), (float)m_ViewportWidth, (float)m_ViewportHeight, 0.1f, 10000.0f));
		camera->OnUpdate(ts);
		render->SubmitScene(camera);
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