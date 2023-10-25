#pragma once

#include <Radiant/Scene/Entity.hpp>
#include <Radiant/Rendering/RenderingPass.hpp>
#include <Radiant/Rendering/Shader.hpp>

namespace Radiant
{
	struct DrawProperties
	{
		Memory::Shared<Mesh> Mesh;
		glm::mat4 Transform;
		Memory::Shared<Material> Material;
	};

	class SceneRendering : public Memory::RefCounted
	{
	public:
		SceneRendering(const Memory::Shared<Scene>& scene);
		~SceneRendering();

		void SetScene(const Memory::Shared<Scene>& scene) { m_Context = scene; }
		void SetSceneVeiwPortSize(const glm::vec2& size);
		
		void SubmitScene(Camera* camera);

		uint32_t GetFinalPassImage(); // TODO(Danya): Set result type Image2D
	private:
		void AddMeshToDrawList(DrawProperties component) const;
		void AddMeshToDrawListWithShader(const Memory::Shared<Mesh>& mesh) const;

		void AddTextureCubeToDrawList(const Memory::Shared<TextureCube>& cube) const;
	private:
		void Init();
	private:
		void CompositePass();
		void GeometryPass();
		void Flush();
	private:
		void DrawSkyLight(const Memory::Shared<TextureCube>& cube);
		void UpdateDirectionalLight(Memory::Shared<Material>& material);
		void UpdateCamera(Camera* camera);
	private:
		std::size_t m_ViewportWidth = -1, m_ViewportHeight = -1;
		Memory::Shared<Scene> m_Context;

		friend Scene;
	};
}