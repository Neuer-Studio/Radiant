#include <Radiant/Rendering/SceneRendering.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glad/glad.h>

namespace Radiant
{
	struct CompositeData
	{
		Memory::Shared<Shader> CompositeShader;
		Memory::Shared<RenderingPass> CompositePass;
		Memory::Shared<Pipeline> CompositePipeline;
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
		std::vector<Memory::Shared<Mesh>> MeshDrawList;
		std::vector<Memory::Shared<Mesh>> MeshDrawListWithShader;

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

		/* Composite Pass */

		{
			FramebufferSpecification FrameBufferComposite;
			FrameBufferComposite.Height = 1;
			FrameBufferComposite.Width = 1;
			FrameBufferComposite.Samples = 1;
			FrameBufferComposite.ClearColor = glm::vec4(0.5f);
			FrameBufferComposite.Format = ImageFormat::RGB8;

			RenderingPassSpecification PassComposite;
			PassComposite.TargetFramebuffer = Framebuffer::Create(FrameBufferComposite);
			s_SceneInfo->CompositeInfo.CompositePass = RenderingPass::Create(PassComposite);

			s_SceneInfo->CompositeInfo.CompositeShader = Shader::Create("Resources/Shaders/hdr.rads");
		}

		/* Geometry Pass */

		{
			FramebufferSpecification FrameBufferGeometry;
			FrameBufferGeometry.Height = 1;
			FrameBufferGeometry.Width = 1;
			FrameBufferGeometry.Samples = 1;
			FrameBufferGeometry.ClearColor = glm::vec4(0.5f); 
			FrameBufferGeometry.Format = ImageFormat::RGB16F;

			RenderingPassSpecification PassGeometry;
			PassGeometry.TargetFramebuffer = Framebuffer::Create(FrameBufferGeometry);
			s_SceneInfo->GeometryInfo.GeometryPass = RenderingPass::Create(PassGeometry);
		}

	}

	void SceneRendering::SubmitScene()
	{
		if (!s_SceneInfo->Camera)
		{
			if (m_Context->ContainsEntityInScene(ComponentType::Camera))
				s_SceneInfo->Camera = &m_Context->GetEntityByComponentType(ComponentType::Camera)->GetComponent(ComponentType::Camera).As<CameraComponent>()->Camera;
		}

		if (s_SceneInfo->Camera) s_SceneInfo->Camera->Update();
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
			for (const auto m : s_SceneInfo->MeshDrawList)
				Rendering::DrawMesh(m);
		}
		Rendering::UnbindRenderingPass();
	}

	void SceneRendering::CompositePass()
	{
		Rendering::BindRenderingPass(s_SceneInfo->CompositeInfo.CompositePass);
		{
			s_SceneInfo->CompositeInfo.CompositeShader->SetValue("u_Exposure", m_Context->m_Exposure, UniformTarget::Fragment);
			s_SceneInfo->GeometryInfo.GeometryPass->GetSpecification().TargetFramebuffer->BindTexture();

			s_SceneInfo->CompositeInfo.CompositeShader->Bind();

			Rendering::DrawFullscreenQuad();
		}
		Rendering::UnbindRenderingPass();
	}

	void SceneRendering::AddMeshToDrawList(Memory::Shared<Mesh> mesh) const
	{
		s_SceneInfo->MeshDrawList.push_back(mesh);
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
	}

	uint32_t SceneRendering::GetFinalPassImage()
	{
		return s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->GetRendererID();
	}
}