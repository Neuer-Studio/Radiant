#include <Radiant/Rendering/Platform/OpenGL/OpenGLFramebuffer.hpp>
#include <Radiant/Rendering/RenderingTypes.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <Radiant/Rendering/Platform/OpenGL/OpenGLUtils.hpp>

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
		Rendering::SubmitCommand([=]()
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

		Rendering::SubmitCommand([this]()
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

				auto format = Utils::OpenGLImageInternalFormat(m_Specification.Format);
				auto type = Utils::OpenGLFormatDataType(m_Specification.Format);

				glTexImage2D(GL_TEXTURE_2D, 0, format, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, type, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

				glGenTextures(1, &m_DepthAttachment);
				glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0,
					GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
				);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);


				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					RADIANT_VERIFY(false, "Framebuffer is incomplete!");
			});

	}

	void OpenGLFramebuffer::Bind() const
	{
		Rendering::SubmitCommand([this]()
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
				glViewport(0, 0, m_Specification.Width, m_Specification.Height);
			});
	}

	void OpenGLFramebuffer::Unbind() const
	{
		Rendering::SubmitCommand([=]()
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			});
	}

	void OpenGLFramebuffer::BindTexture(uint32_t attachmentIndex, uint32_t slot) const
	{
		Rendering::SubmitCommand([=]()
			{
				glActiveTexture(GL_TEXTURE0 + slot);
				glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
			});
	}
}