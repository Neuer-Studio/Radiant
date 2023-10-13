#include <Radiant/Rendering/SceneRendering.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <glm/ext/matrix_transform.hpp>

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

		/* Composite Pass */

		FramebufferSpecification FrameBufferComposite;
		FrameBufferComposite.Height = 1;
		FrameBufferComposite.Width = 1;
		FrameBufferComposite.Samples = 1;
		FrameBufferComposite.ClearColor = glm::vec4(0.5f);

		RenderingPassSpecification PassComposite;
		PassComposite.TargetFramebuffer = Framebuffer::Create(FrameBufferComposite);

		s_SceneInfo->CompositeInfo.CompositePass = RenderingPass::Create(PassComposite);

		s_SceneInfo->CompositeInfo.CompositeShader = Shader::Create("Resources/Shaders/Colors.rads");

		/****************************/

		/* Geometry Pass */




		/****************************/

	}

	void SceneRendering::SubmitScene()
	{
		if (!s_SceneInfo->Camera)
		{
			if(m_Context->ContainsEntityInScene(ComponentType::Camera))
				s_SceneInfo->Camera = &m_Context->GetEntityByComponentType(ComponentType::Camera)->GetComponent(ComponentType::Camera).As<CameraComponent>()->Camera;
		}

 		if (s_SceneInfo->Camera) s_SceneInfo->Camera->Update();
		Flush();
	}

	void SceneRendering::Flush()
	{
		if (s_SceneInfo->Camera)
		{
			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.f));
			glm::mat4 viewMatrix = s_SceneInfo->Camera->GetViewMatrix();
			glm::mat4 projectionMatrix = s_SceneInfo->Camera->GetProjectionMatrix();

			s_SceneInfo->CompositeInfo.CompositeShader->SetValue("u_Model", (std::byte*)&modelMatrix, UniformTarget::Vertex);
			s_SceneInfo->CompositeInfo.CompositeShader->SetValue("u_View", (std::byte*)&viewMatrix, UniformTarget::Vertex);
			s_SceneInfo->CompositeInfo.CompositeShader->SetValue("u_Projection", (std::byte*)&projectionMatrix, UniformTarget::Vertex);
			s_SceneInfo->CompositeInfo.CompositeShader->Bind();
		}
		CompositePass();

		s_SceneInfo->MeshDrawList.clear();
		s_SceneInfo->MeshDrawListWithShader.clear();
	}

	void SceneRendering::CompositePass()
	{
		Rendering::BindRenderingPass(s_SceneInfo->CompositeInfo.CompositePass);


		for (const auto m : s_SceneInfo->MeshDrawList)
			Rendering::DrawMesh(m);


		//s_SceneInfo->GeometryInfo.GeometryPass->GetSpecification().TargetFramebuffer->BindTexture();
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
	//		s_SceneInfo->GeometryInfo.GeometryPass->GetSpecification().TargetFramebuffer->Resize(size.x, size.y);
		}
	}

	uint32_t SceneRendering::GetFinalPassImage()
	{
		return s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->GetRendererID();
	}
}