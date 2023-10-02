#include <Radiant/Scene/Scene.hpp>

namespace Radiant
{
	Memory::Ref<Scene> SceneManager::Create(const std::string& SceneName)
	{
		return Memory::Ref<Scene>::Create(SceneName);
	}
}