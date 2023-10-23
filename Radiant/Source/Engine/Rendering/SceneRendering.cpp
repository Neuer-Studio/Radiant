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
		Memory::Shared<class Material> QuadMaterial;

		Camera* Camera;
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
			FrameBufferComposite.Height = 1;
			FrameBufferComposite.Width = 1;
			FrameBufferComposite.Samples = 1;
			FrameBufferComposite.ClearColor = glm::vec4(0.5f);
			FrameBufferComposite.Format = ImageFormat::RGBA8;

			RenderingPassSpecification PassComposite;
			PassComposite.TargetFramebuffer = Framebuffer::Create(FrameBufferComposite);
			s_SceneInfo->CompositeInfo.CompositePass = RenderingPass::Create(PassComposite);

			s_SceneInfo->CompositeInfo.CompositeShader = Shader::Create("Resources/Shaders/hdr.rads");
			s_SceneInfo->CompositeInfo.CompositeMaterial = Material::Create(s_SceneInfo->CompositeInfo.CompositeShader);
		}

		/* Geometry Pass */

		{
			FramebufferSpecification FrameBufferGeometry;
			FrameBufferGeometry.Height = 1;
			FrameBufferGeometry.Width = 1;
			FrameBufferGeometry.Samples = 1;
			FrameBufferGeometry.ClearColor = glm::vec4(0.5f); 
			FrameBufferGeometry.Format = ImageFormat::RGB16AF;

			RenderingPassSpecification PassGeometry;
			PassGeometry.TargetFramebuffer = Framebuffer::Create(FrameBufferGeometry);
			s_SceneInfo->GeometryInfo.GeometryPass = RenderingPass::Create(PassGeometry);
		}

	}

	void SceneRendering::SubmitScene()
	{
		UpdateCamera();
		Flush();
	}

	void SceneRendering::Flush()
	{
		GeometryPass();
		CompositePass();

		s_SceneInfo->MeshDrawList.clear();
		s_SceneInfo->MeshDrawListWithShader.clear();
	}

	void SceneRendering::GeometryPass()
	{
		Rendering::BindRenderingPass(s_SceneInfo->GeometryInfo.GeometryPass);
		{
			DrawSkyLight();
			for (auto dc : s_SceneInfo->MeshDrawList)
			{
				glm::mat4 viewProjection;
				glm::vec3 CameraPosition;

				Memory::Shared<Material> material;
				material = Material::Create(s_SceneInfo->StaticShader);

				if (s_SceneInfo->Camera)
				{
					viewProjection = s_SceneInfo->Camera->GetProjectionMatrix() * s_SceneInfo->Camera->GetViewMatrix();
					CameraPosition = s_SceneInfo->Camera->GetPosition();
				}
				material->SetValue("u_ViewProjection", viewProjection, UniformTarget::Vertex);
				material->SetValue("u_Transform", dc.Transform, UniformTarget::Vertex);
				material->SetValue("u_CameraPosition", CameraPosition, UniformTarget::Fragment);

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

	void SceneRendering::SetSceneVeiwPortSize(const glm::vec2& size)
	{
		if (m_ViewportWidth != size.x || m_ViewportHeight != size.y)
		{
			m_ViewportWidth = size.x;
			m_ViewportHeight = size.y;

			s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->Resize(size.x, size.y);
			s_SceneInfo->GeometryInfo.GeometryPass->GetSpecification().TargetFramebuffer->Resize(size.x, size.y);
		}
		if(s_SceneInfo->Camera)
			s_SceneInfo->Camera->SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), size.x, size.y, 0.1f, 10000.0f));
	}

	uint32_t SceneRendering::GetFinalPassImage()
	{
		return s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->GetRendererID();
	}

	void SceneRendering::DrawSkyLight() // TODO(Danya): Fix crush
	{
		if (!m_Context->ContainsEntityInScene(ComponentType::Cube))
			return;
		auto cube = m_Context->GetEntityByComponentType(ComponentType::Cube)->GetComponent(ComponentType::Cube).As<CubeComponent>()->Cube;
		glm::mat4 viewProjection;

		if(s_SceneInfo->Camera)
			viewProjection = s_SceneInfo->Camera->GetViewProjectionMatrix();
		s_SceneInfo->QuadShader->Bind();
		Rendering::DrawFullscreenQuad(s_SceneInfo->QuadMaterial);
		cube->Bind();
		s_SceneInfo->QuadMaterial->SetValue("u_InverseVP", glm::inverse(viewProjection), UniformTarget::Vertex);
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

	void SceneRendering::UpdateCamera()
	{
		if (!s_SceneInfo->Camera)
		{
			if (m_Context->ContainsEntityInScene(ComponentType::Camera))
				s_SceneInfo->Camera = &m_Context->GetEntityByComponentType(ComponentType::Camera)->GetComponent(ComponentType::Camera).As<CameraComponent>()->Camera;
		}

		if (s_SceneInfo->Camera) s_SceneInfo->Camera->Update();
	}

}