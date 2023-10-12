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

		void SubmitScene();

		uint32_t GetFinalPassImage(); // TODO(Danya): Set result type Image2D

		void AddMeshToDrawList(const Memory::Shared<Mesh>& mesh) const;
		void AddMeshToDrawListWithShader(const Memory::Shared<Mesh>& mesh) const;
	private:
		void CompositePass();

		void Flush();
	private:
		Memory::Shared<Scene> m_Context;
	};
}