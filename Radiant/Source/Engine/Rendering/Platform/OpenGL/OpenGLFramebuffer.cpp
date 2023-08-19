#include <Radiant/Rendering/Platform/OpenGL/OpenGLFramebuffer.hpp>
#include <Radiant/Rendering/RenderingTypes.hpp>
#include <Radiant/Rendering/Rendering.hpp>

#include <glad/glad.h>

namespace Radiant
{
	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		Resize(spec.Width, spec.Height);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		Rendering::Submit([=]()
			{
				glDeleteFramebuffers(1, &m_RendererID);
			});
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate /* = false */)
	{
		if (forceRecreate)
			return;
		m_Specification.Width = width;
		m_Specification.Height = height;


		Rendering::Submit([this]()
			{
				if (m_RendererID)
				{
					glDeleteFramebuffers(1, &m_RendererID);
					glDeleteTextures(1, &m_ColorAttachment);
					glDeleteTextures(1, &m_DepthAttachment);
				}

				
				glGenFramebuffers(1, &m_RendererID);
				glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

				glGenTextures(1, &m_ColorAttachment);
				glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Specification.Width, m_Specification.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);


				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					RADIANT_VERIFY(false, "Framebuffer is incomplete!");
			});

	}

	void OpenGLFramebuffer::Bind() const
	{
		Rendering::Submit([this]()
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
				glViewport(0, 0, m_Specification.Width, m_Specification.Height);
			});
	}

	void OpenGLFramebuffer::Unbind() const
	{
		Rendering::Submit([this]()
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			});
	}

	void OpenGLFramebuffer::BindTexture(uint32_t attachmentIndex, uint32_t slot) const
	{
		Rendering::Submit([=]()
			{
				glActiveTexture(GL_TEXTURE0 + slot);
				glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
			});
	}
}