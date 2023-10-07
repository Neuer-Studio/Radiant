#pragma once

#include <Radiant/Rendering/Framebuffer.hpp>

namespace Radiant
{
	class OpenGLFramebuffer final: public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);

		virtual ~OpenGLFramebuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;

		virtual void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const override;

		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }

		virtual RendererID GetRendererID() const override { return m_ColorAttachment; } //NOTE: Temp

		virtual Memory::Shared<Image2D> GetImage() const override { return {}; }
		virtual Memory::Shared<Image2D> GetDepthImage() const override { return {}; }
	private:
		FramebufferSpecification m_Specification;
		RendererID m_ColorAttachment, m_DepthAttachment;
		RendererID m_RendererID;
	};
}