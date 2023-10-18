#include <Radiant/Rendering/Platform/OpenGL/OpenGLRenderingPass.hpp>
#include <Radiant/Rendering/Rendering.hpp>

namespace Radiant
{
	OpenGLRenderingPass::OpenGLRenderingPass(const RenderingPassSpecification& spec)
		: m_Specification(spec)
	{

	}

	OpenGLRenderingPass::~OpenGLRenderingPass()
	{

	}

	void OpenGLRenderingPass::Bind() const
	{
		m_Specification.TargetFramebuffer->Bind();

		const auto fbspec = m_Specification.TargetFramebuffer->GetSpecification();

		if (m_Specification.clear)
			Rendering::Clear(fbspec.ClearColor.r, fbspec.ClearColor.g, fbspec.ClearColor.b);
	}

	void OpenGLRenderingPass::Unbind() const
	{
		m_Specification.TargetFramebuffer->Unbind();
	}

}