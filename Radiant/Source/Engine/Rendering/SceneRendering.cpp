#include <Radiant/Rendering/SceneRendering.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glad/glad.h>

#include <Radiant/Rendering/Material.hpp>

namespace Radiant
{
	struct CompositeData
	{
		Memory::Shared<Shader> CompositeShader;
		Memory::Shared<RenderingPass> CompositePass;
		Memory::Shared<Pipeline> CompositePipeline;
		Memory::Shared<class Material> CompositeMaterial;
	};

	struct GeometryData
	{
		Memory::Shared<Shader> GeometryShader;
		Memory::Shared<RenderingPass> GeometryPass;
		Memory::Shared<Pipeline> GeometryPipeline;
	};

	struct SceneInfo
	{
		CompositeData CompositeInfo;
		GeometryData GeometryInfo;

		Memory::Shared<Shader> QuadShader;
		Memory::Shared<Shader> StaticShader;
		std::vector<DrawProperties> MeshDrawList;
		std::vector<Memory::Shared<Mesh>> MeshDrawListWithShader;
		std::vector<Memory::Shared<TextureCube>> TextureCubeList;
		Memory::Shared<class Material> QuadMaterial;

		uint32_t Samples;

		struct
		{
			glm::mat4 ViewProjection;
			glm::vec3 CameraPosition;
		} Camera;
	};

	static SceneInfo *s_SceneInfo = nullptr;

	SceneRendering::SceneRendering(const Memory::Shared<Scene>& scene)
		: m_Context(scene)
	{
		Init();
	}

	SceneRendering::~SceneRendering()
	{
		delete s_SceneInfo;
	}

	void SceneRendering::Init()
	{
		s_SceneInfo = new SceneInfo();
		s_SceneInfo->QuadShader = Rendering::GetShaderLibrary()->Get("Quad.rads");
		s_SceneInfo->StaticShader = Rendering::GetShaderLibrary()->Get("Static_Shader.rads");
		s_SceneInfo->QuadMaterial = Material::Create(s_SceneInfo->QuadShader);
		/* Composite Pass */

		{
			FramebufferSpecification FrameBufferComposite;
			FrameBufferComposite.ClearColor = glm::vec4(0.5f);
			FrameBufferComposite.Format = ImageFormat::RGBA8;

			RenderingPassSpecification PassComposite;
			PassComposite.TargetFramebuffer = Framebuffer::Create(FrameBufferComposite);
			s_SceneInfo->CompositeInfo.CompositePass = RenderingPass::Create(PassComposite);

			s_SceneInfo->CompositeInfo.CompositeShader = Shader::Create("Resources/Shaders/Scene.rads");
			s_SceneInfo->CompositeInfo.CompositeMaterial = Material::Create(s_SceneInfo->CompositeInfo.CompositeShader);
		}

		/* Geometry Pass */

		{
			FramebufferSpecification FrameBufferGeometry;
			FrameBufferGeometry.ClearColor = glm::vec4(0.5f); 
			FrameBufferGeometry.Format = ImageFormat::RGBA16F;
			s_SceneInfo->Samples = FrameBufferGeometry.Samples = 16;

			RenderingPassSpecification PassGeometry;
			PassGeometry.TargetFramebuffer = Framebuffer::Create(FrameBufferGeometry);
			s_SceneInfo->GeometryInfo.GeometryPass = RenderingPass::Create(PassGeometry);
		}

	}

	void SceneRendering::SubmitScene(Camera* camera)
	{
		if(camera)
			UpdateCamera(camera);
		Flush();
	}

	void SceneRendering::Flush()
	{
		GeometryPass();
		CompositePass();

		s_SceneInfo->MeshDrawList.clear();
		s_SceneInfo->MeshDrawListWithShader.clear();
		s_SceneInfo->TextureCubeList.clear();
	}

	void SceneRendering::GeometryPass()
	{
		static auto texture = Texture2D::Create("Resources/Meshes/Laminate floor/textures/laminate_floor_02_diff_4k.jpg");
		Rendering::BindRenderingPass(s_SceneInfo->GeometryInfo.GeometryPass);
		{
			for(const auto& cube : s_SceneInfo->TextureCubeList)
				DrawSkyLight(cube);

			for (auto& dc : s_SceneInfo->MeshDrawList)
			{
				Memory::Shared<Material>& material = dc.Material;

				material->SetValue("u_Transform", dc.Transform, UniformTarget::Vertex);
				material->SetValue("u_ViewProjection", s_SceneInfo->Camera.ViewProjection, UniformTarget::Vertex);

				material->SetValue("u_CameraPosition", s_SceneInfo->Camera.CameraPosition, UniformTarget::Fragment);
				material->SetValue("u_AlbedoTexture", texture);

				UpdateDirectionalLight(material);
				Rendering::DrawMesh(dc.Mesh, material);
			}
		}
		Rendering::UnbindRenderingPass();
	}

	void SceneRendering::CompositePass()
	{
		Rendering::BindRenderingPass(s_SceneInfo->CompositeInfo.CompositePass);
		{
			s_SceneInfo->CompositeInfo.CompositeMaterial->SetValue("u_Exposure", m_Context->m_Exposure, UniformTarget::Fragment);
			s_SceneInfo->CompositeInfo.CompositeMaterial->SetValue("u_TextureSamples", (uint32_t)s_SceneInfo->Samples, UniformTarget::Fragment);
			s_SceneInfo->GeometryInfo.GeometryPass->GetSpecification().TargetFramebuffer->BindTexture();

		}
		Rendering::DrawFullscreenQuad(s_SceneInfo->CompositeInfo.CompositeMaterial);
		Rendering::UnbindRenderingPass();
	}

	void SceneRendering::AddMeshToDrawList(DrawProperties component) const
	{
		s_SceneInfo->MeshDrawList.push_back(component);
	}

	void SceneRendering::AddMeshToDrawListWithShader(const Memory::Shared<Mesh>& mesh) const
	{
		s_SceneInfo->MeshDrawListWithShader.push_back(mesh);
	}

	void SceneRendering::AddTextureCubeToDrawList(const Memory::Shared<TextureCube>& cube) const
	{
		s_SceneInfo->TextureCubeList.push_back(cube);
	}

	void SceneRendering::SetSceneVeiwPortSize(const glm::vec2& size)
	{
		if (m_ViewportWidth != size.x || m_ViewportHeight != size.y)
		{
			m_ViewportWidth = size.x;
			m_ViewportHeight = size.y;

			s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->Resize(size.x, size.y);
			s_SceneInfo->GeometryInfo.GeometryPass->GetSpecification().TargetFramebuffer->Resize(size.x, size.y);


			if (m_Context->ContainsEntityInScene(ComponentType::Camera))
			{
				auto& camera = m_Context->GetEntityByComponentType(ComponentType::Camera)->GetComponent(ComponentType::Camera).As<CameraComponent>()->Camera;
				camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), size.x, size.y, 0.1f, 10000.0f));
			}

		}
	}

	uint32_t SceneRendering::GetFinalPassImage()
	{
		return s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->GetRendererID();
	}

	void SceneRendering::DrawSkyLight(const Memory::Shared<TextureCube>& cube) // TODO(Danya): Fix crush(Done)
	{
		s_SceneInfo->QuadShader->Bind();
		Rendering::DrawFullscreenQuad(s_SceneInfo->QuadMaterial);
		cube->Bind();
		s_SceneInfo->QuadMaterial->SetValue("u_InverseVP", glm::inverse(s_SceneInfo->Camera.ViewProjection), UniformTarget::Vertex);
		Rendering::DrawIndexed(6);
	}

	void SceneRendering::UpdateDirectionalLight(Memory::Shared<Material>& material)
	{
		if (m_Context->ContainsEntityInScene(ComponentType::DirectionLight))
		{
			auto dirLight = m_Context->GetEntityByComponentType(ComponentType::DirectionLight)->GetComponent(ComponentType::DirectionLight).As<DirectionLightComponent>()->DirLight;
			material->SetValue("u_Light.Direction", dirLight.Direction, UniformTarget::Fragment);
			material->SetValue("u_Light.Radiance", dirLight.Radiance, UniformTarget::Fragment);
		}
	}

	void SceneRendering::UpdateCamera(Camera* camera)
	{
		s_SceneInfo->Camera.ViewProjection = camera->GetViewProjectionMatrix();
		s_SceneInfo->Camera.CameraPosition = camera->GetPosition();

		camera->Update();
	}

	uint32_t SceneRendering::GetSamplesCount()
	{
		return s_SceneInfo->Samples;
	}

	void SceneRendering::SetSampelsCount(uint32_t count)
	{
		s_SceneInfo->Samples = count;
	}

}