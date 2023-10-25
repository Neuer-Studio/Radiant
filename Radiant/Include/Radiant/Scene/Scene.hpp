#pragma once

#include <Radiant/Utilities/UUID.hpp>
#include <glm/glm.hpp>

namespace Radiant
{
	class SceneRendering;
	class SceneRenderingPanel;
	class SceneManager;
	class PanelOutliner;
	class Entity;
	enum class ComponentType;

	enum class SceneType
	{
		None = 0, Editor, RunTime
	};

	class Scene : public Memory::RefCounted
	{
	public:
		Scene(const std::string& SceneName);
		~Scene(); //NOTE(Danya): Should call destructor for Entitys

		void AddEntity(const Entity* entity);
		Entity* CreateEntity(const std::string& name = "Debug Name");
		Entity* GetEntityByUUID(uint64_t uuid);
		Entity* GetEntityByComponentType(ComponentType type);
		bool ContainsEntityInScene(ComponentType type);

		std::vector<Entity*> GetEntityList() { return m_Entitys; }

		void UpdateScene(const Memory::Shared<SceneRendering>& rendering, SceneType type = SceneType::Editor);
	private:
		Entity* m_SelectedEntity = nullptr; // NOTE(Danya): Using for panels
		std::vector<Entity*> m_Entitys;

		Memory::Shared<SceneRendering> m_SceneRendering;

		std::string m_SceneName;
		Utils::UUID m_UUID;

		std::size_t m_ViewportWidth = 0;
		std::size_t m_ViewportHeight = 0;

		float m_Exposure = 1.0f; // NOTE(Danya): Using in SceneRendering

		friend SceneManager;
		friend SceneRenderingPanel;
		friend SceneRendering;
		friend PanelOutliner;
	};

	class SceneManager
	{
	public:
		SceneManager(const std::string& name);
		Memory::Shared<Scene> Create(const std::string& SceneName = "Debug Name"); // Create a new scene
		void SetActiveScene(const Memory::Shared<Scene>& scene) { m_ActiveScene = scene; }
		Memory::Shared<Scene> GetSceneByName(const std::string& name);
	private:
		std::string m_ManagerName;
		Memory::Shared<Scene> m_ActiveScene;
		std::vector<Memory::Shared<Scene>> m_ScenePool;
	};
}