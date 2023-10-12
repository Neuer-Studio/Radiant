#include <Radiant/Rendering/SceneRendering.hpp>
#include <Radiant/Rendering/Rendering.hpp>

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
		s_SceneInfo->MeshDrawList.clear();
		CompositePass();
	}

	void SceneRendering::CompositePass()
	{
		Rendering::BindRenderingPass(s_SceneInfo->CompositeInfo.CompositePass);


		//s_SceneInfo->GeometryInfo.GeometryPass->GetSpecification().TargetFramebuffer->BindTexture();
		Rendering::UnbindRenderingPass();
	}

	void SceneRendering::AddMeshToDrawList(const Memory::Shared<Mesh>& mesh) const
	{
		s_SceneInfo->MeshDrawList.push_back(mesh);
	}

	uint32_t SceneRendering::GetFinalPassImage()
	{
		return s_SceneInfo->CompositeInfo.CompositePass->GetSpecification().TargetFramebuffer->GetImage();
	}
}