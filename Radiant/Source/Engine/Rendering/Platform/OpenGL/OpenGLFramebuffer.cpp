#include <Radiant/Rendering/Platform/OpenGL/OpenGLFramebuffer.hpp>
#include <Radiant/Rendering/RenderingTypes.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <Radiant/Rendering/Platform/OpenGL/OpenGLUtils.hpp>

#include <glad/glad.h>

namespace Radiant
{

	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, RendererID* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), 1, outID);
		}

		static void BindTexture(bool multisampled, RendererID id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(RendererID id, int samples, GLenum iformat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, iformat, width, height, GL_FALSE);
			}
			else
			{
				// Only RGBA access for now
				glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, GL_RGBA, format, nullptr);

				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(RendererID id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}
	}

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

		Memory::Shared<OpenGLFramebuffer> instance = this;
		Rendering::SubmitCommand([instance]()
			{
				bool multisampled = instance->m_Specification.Samples > 1;
				int samples = instance->m_Specification.Samples;

				if (instance->m_RendererID)
				{
					glDeleteFramebuffers(1, &instance->m_RendererID);
					glDeleteTextures(1, &instance->m_ColorAttachment);
					glDeleteTextures(1, &instance->m_DepthAttachment);
				}

				glGenFramebuffers(1, (GLuint*) & instance->m_RendererID);
				glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);

				auto InternalFormat = Utils::OpenGLImageInternalFormat(instance->m_Specification.Format);
				auto type = Utils::OpenGLFormatDataType(instance->m_Specification.Format);

				Utils::CreateTextures(multisampled, (RendererID*) & instance->m_ColorAttachment, 1);
				Utils::BindTexture(multisampled, instance->m_ColorAttachment);

				Utils::AttachColorTexture(instance->m_ColorAttachment, samples, InternalFormat, type, instance->m_Specification.Width, instance->m_Specification.Height, 0);
			
				Utils::CreateTextures(multisampled, (RendererID*)&instance->m_DepthAttachment, 1);
				Utils::BindTexture(multisampled, instance->m_DepthAttachment);

				Utils::AttachDepthTexture(instance->m_DepthAttachment, samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, instance->m_Specification.Width, instance->m_Specification.Height);

				RADIANT_VERIFY(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

	void OpenGLFramebuffer::BindTexture(uint32_t slot) const
	{
		Rendering::SubmitCommand([=]()
			{
				glBindTextureUnit(slot, m_ColorAttachment);
			});
	}
}