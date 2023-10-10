#pragma once

#include <Radiant/Scene/Entity.hpp>
#include <Radiant/Rendering/RenderingPass.hpp>
#include <Radiant/Rendering/Shader.hpp>

namespace Radiant
{
	class SceneRendering
	{
		SceneRendering(const Memory::Shared<Scene>& scene);
		~SceneRendering();

		void SetScene(const Memory::Shared<Scene>& scene) { m_Context = scene; }
		void Init();

		void Begin();
		void End();

	private:
		void Flush();
	private:
		Memory::Shared<Scene> m_Context;

		Memory::Shared<Shader> m_CompositeShader;
		Memory::Shared<RenderingPass> m_CompositePass;

		bool m_Active;
	};
}