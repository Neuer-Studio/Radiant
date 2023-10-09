#pragma once

#include <Radiant/Utilities/UUID.hpp>

namespace Radiant
{
	class SceneManager;
	class Entity;

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

		std::vector<Entity*> GetEntityList() { return m_Entitys; }

		void UpdateScene(SceneType type);
	private:
		std::vector<Entity*> m_Entitys;
		std::string m_SceneName;
		Utils::UUID m_UUID;

		friend SceneManager;
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