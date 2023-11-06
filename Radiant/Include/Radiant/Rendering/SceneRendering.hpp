#pragma once

#include <Radiant/Scene/Entity.hpp>
#include <Radiant/Rendering/RenderingPass.hpp>
#include <Radiant/Rendering/Shader.hpp>
#include <Radiant/Rendering/Environment.hpp>

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
		void SetEnvRadiance(const Memory::Shared<TextureCube>& envRadiance) const;
		void SetEnvIrradiance(const Memory::Shared<TextureCube>& envIrradiance) const;
		void SetEnvironment(const Environment& environment) const;
		static std::pair<Memory::Shared<TextureCube>, Memory::Shared<TextureCube>> CreateEnvironmentMap(const std::string& filepath);
		void SubmitScene(Camera* camera);

		Memory::Shared<Image2D> GetFinalPassImage(); // TODO(Danya): Set result type Image2D
		Memory::Shared<Image2D> GetFinalDepthImage(); // TODO(Danya): Set result type Image2D

		uint32_t GetSamplesCount();
		void SetSampelsCount(uint32_t count = 2u);
		float GetSkyboxLod();
		void SetSkyboxLod(float value = 0.0f);
	private:
		void AddMeshToDrawList(DrawProperties component) const;
		void AddMeshToDrawListWithShader(const Memory::Shared<Mesh>& mesh) const;
	private:
		void Init();
	private:
		void CompositePass();
		void GeometryPass();
		void Flush();
	private:
		void UpdateSkyLight();
		void UpdateDirectionalLight(Memory::Shared<Material>& material);
	private:
		std::size_t m_ViewportWidth = -1, m_ViewportHeight = -1;
		Memory::Shared<Scene> m_Context;
		friend Scene;
		friend SceneRenderingPanel;
	};
}