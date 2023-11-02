#include <Radiant/Rendering/Platform/OpenGL/OpenGLFramebuffer.hpp>
#include <Radiant/Rendering/RenderingTypes.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <Radiant/Rendering/Platform/OpenGL/OpenGLUtils.hpp>
#include <Radiant/Rendering/Platform/OpenGL/OpenGLImage.hpp>

#include <glad/glad.h>

namespace Radiant
{

	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static Memory::Shared<Image2D> CreateAndAttachColorTexture(int samples, ImageFormat format, uint32_t width, uint32_t height, int index)
		{
			auto image = Image2D::Create(format, width, height, nullptr, samples);
			image->Invalidate();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(samples > 1), image.As<OpenGLImage2D>()->GetRenderingID(), 0);

			return image;
		}

		static Memory::Shared<Image2D> CreateAndAttachDepthTexture(int samples, ImageFormat format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			auto image = Image2D::Create(format, width, height, nullptr, samples);
			image->Invalidate();

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), image.As<OpenGLImage2D>()->GetRenderingID(), 0);

			return image;
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
		Rendering::SubmitCommand([instance]() mutable
			{
				bool multisampled = instance->m_Specification.Samples > 1;
				int samples = instance->m_Specification.Samples;

				if (instance->m_RendererID)
				{
					glDeleteFramebuffers(1, &instance->m_RendererID);
				}

				if (instance->m_ColorAttachment)
				{
					auto id = instance->m_ColorAttachment->GetImageID();
					glDeleteTextures(1, (GLuint*)&id);
				}

				if (instance->m_DepthAttachment)
				{
					auto id = instance->m_DepthAttachment->GetImageID();
					glDeleteTextures(1, (GLuint*)&id);
				}

				glGenFramebuffers(1, (GLuint*) & instance->m_RendererID);
				glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);

				instance->m_ColorAttachment = Utils::CreateAndAttachColorTexture(samples, instance->m_Specification.Format, instance->m_Specification.Width, instance->m_Specification.Height, 0);
				instance->m_DepthAttachment = Utils::CreateAndAttachDepthTexture(samples, ImageFormat::DEPTH24STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, instance->m_Specification.Width, instance->m_Specification.Height);

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
				glBindTextureUnit(slot, m_ColorAttachment->GetImageID());
			});
	}
}