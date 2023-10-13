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
		void SetSceneVeiwPortSize(const glm::vec2& size);
		
		void SubmitScene();

		uint32_t GetFinalPassImage(); // TODO(Danya): Set result type Image2D
	private:
		void AddMeshToDrawList(Memory::Shared<Mesh> mesh) const;
		void AddMeshToDrawListWithShader(const Memory::Shared<Mesh>& mesh) const;
	private:
		void Init();
		void CompositePass();
		void Flush();
	private:
		std::size_t m_ViewportWidth = -1, m_ViewportHeight = -1;
		Memory::Shared<Scene> m_Context;

		friend Scene;
	};
}