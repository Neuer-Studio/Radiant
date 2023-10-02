#pragma once

#include <Radiant/Scene/Entity.hpp>

namespace Radiant
{
	class SceneManager;

	class Scene : public Memory::RefCounted
	{
	private:
		Scene(const std::string& SceneName);
		~Scene();
	private:
		std::vector<Entity*> m_Entitys;
		std::string m_SceneName;

		friend SceneManager;
	};

	class SceneManager
	{
	public:
		Memory::Ref<Scene> Create(const std::string& SceneName = "Debug Name");
		void SetActiveScene(Memory::Ref<Scene> scene) { m_ActiveScene = scene; }
	private:
		Memory::Ref<Scene> m_ActiveScene;
	};
}