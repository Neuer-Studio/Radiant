#pragma once

#include <Radiant/Utilities/UUID.hpp>

namespace Radiant
{
	class SceneManager;
	class Entity;

	class Scene : public Memory::RefCounted
	{
	public:
		Scene(const std::string& SceneName);
		~Scene(); //NOTE(Danya): Should call destructor for Entitys

		void AddEntity(const Entity* entity);
		Entity* CreateEntity(const std::string& name = "Debug Name");
		Entity* GetEntityByUUID(uint64_t uuid);
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
		Memory::Ref<Scene> Create(const std::string& SceneName = "Debug Name");
		void SetActiveScene(const Memory::Ref<Scene>& scene) { m_ActiveScene = scene; }
	private:
		std::string m_ManagerName;
		Memory::Ref<Scene> m_ActiveScene;
	};
}