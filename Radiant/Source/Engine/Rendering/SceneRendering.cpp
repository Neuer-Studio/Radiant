#include <Radiant/Rendering/SceneRendering.hpp>

namespace Radiant
{
	SceneRendering::SceneRendering(const Memory::Shared<Scene>& scene)
		: m_Context(scene)
	{
		Init();
	}

	void SceneRendering::Init()
	{
		/* Composite Pass */

		FramebufferSpecification FrameBufferComposite;
		FrameBufferComposite.Height = 1;
		FrameBufferComposite.Width = 1;
		FrameBufferComposite.Samples = 1;

		RenderingPassSpecification PassComposite;
		PassComposite.Framebuffer = Memory::Shared<Framebuffer>::Create(FrameBufferComposite);

		m_CompositePass = Memory::Shared<RenderingPass>::Create(PassComposite);

		m_CompositeShader = Memory::Shared<Shader>::Create("Resources/Shaders/Colors.rads");

		/****************************/
	}
}