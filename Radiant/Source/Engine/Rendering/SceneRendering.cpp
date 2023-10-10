#include <Radiant/Rendering/SceneRendering.hpp>

namespace Radiant
{
	struct SceneInfo
	{
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

		m_CompositePass = RenderingPass::Create(PassComposite);

		m_CompositeShader = Shader::Create("Resources/Shaders/Colors.rads");

		/****************************/
	}

	void SceneRendering::Begin()
	{
		RADIANT_VERIFY(m_Active);
		if (!s_SceneInfo->Camera) s_SceneInfo->Camera = &m_Context->GetEntityByComponentType(ComponentType::Camera)->GetComponent(ComponentType::Camera).As<CameraComponent>()->Camera;
		m_Active = true;
	}

	void SceneRendering::Flush()
	{

	}

	void SceneRendering::End()
	{
		Flush();
		m_Active = false;
	}

	void SceneRendering::CompositePass()
	{

	}
}