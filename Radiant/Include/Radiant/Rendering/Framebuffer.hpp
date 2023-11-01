#pragma once

#include <Radiant/Rendering/Image.hpp>
#include <Radiant/Rendering/RenderingTypes.hpp>

#include <glm/glm.hpp>

namespace Radiant
{
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(ImageFormat format) : Format(format) {}

		ImageFormat Format;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 1920;
		uint32_t Height = 1080;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		uint32_t Samples = 1; // multisampling
		bool NoResize = false;

		ImageFormat Format;
	};

	class Framebuffer : public Memory::RefCounted
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) = 0;

		virtual void BindTexture(uint32_t slot = 0) const = 0;

		virtual FramebufferSpecification GetSpecification() = 0;
		virtual const FramebufferSpecification GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual RendererID GetRendererID() const = 0;

		virtual Memory::Shared<Image2D> GetImage() const = 0;
		virtual Memory::Shared<Image2D> GetDepthImage() const = 0;

		static Memory::Shared<Framebuffer> Create(const FramebufferSpecification& spec);
	};

	class FramebufferPool final
	{
	public:
		static void Add(Memory::Shared<Framebuffer> framebuffer);

		static std::vector<Memory::Shared<Framebuffer>>& GetAll() { return m_Pool; }
	private:
		static std::vector<Memory::Shared<Framebuffer>> m_Pool;
	};
}