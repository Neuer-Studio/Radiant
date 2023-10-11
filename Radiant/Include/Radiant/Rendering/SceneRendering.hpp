#pragma once

#include <Radiant/Scene/Entity.hpp>
#include <Radiant/Rendering/RenderingPass.hpp>
#include <Radiant/Rendering/Shader.hpp>

namespace Radiant
{
	class SceneRendering : public Memory::RefCounted
	{
	public:
		SceneRendering(const Memory::Shared<Scene>& scene);
		~SceneRendering();

		void SetScene(const Memory::Shared<Scene>& scene) { m_Context = scene; }
		void Init();

		void Begin();
		void End();

		void AddMeshToDrawList(const Memory::Shared<Mesh>& mesh) const;
		void AddMeshToDrawListWithShader(const Memory::Shared<Mesh>& mesh);
	private:
		void CompositePass();

		void Flush();
	private:
		Memory::Shared<Scene> m_Context;

		bool m_Active;
	};
}