#pragma once

#include <Radiant/Rendering/Image.hpp>
#include <Rendering/RenderingTypes.hpp>

#include <glad/glad.h>

namespace Radiant
{
	class OpenGLImage2D final : public Image2D
	{
	public:
		OpenGLImage2D(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer, uint32_t samples);
		OpenGLImage2D(ImageFormat format, std::size_t width, std::size_t height, const void* data, uint32_t samples);
		virtual ~OpenGLImage2D() override;

		virtual void Release() override;
		virtual void Invalidate() override;

		void Invalidate2D();
		void Invalidate2DMS();

		RendererID GetRenderingID() const { return m_RenderingID; }
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual ImageFormat GetFormat() const override { return m_Format; }

		virtual Memory::Buffer GetBuffer() const override { return m_ImageData; }

		virtual uint64_t GetImageID() const override { return m_RenderingID; }
	private:
		ImageFormat m_Format;
		std::uint32_t m_Width;
		std::uint32_t m_Height;
		std::uint32_t m_SamplersCount = 1;
		Memory::Buffer m_ImageData;

		RendererID m_RenderingID = 0;
		RendererID m_SamplerRenderingID = 0;
	};

	class OpenGLImageCube final : public ImageCube
	{
	public:
		OpenGLImageCube(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer);
		OpenGLImageCube(ImageFormat format, std::size_t width, std::size_t height, const std::byte* data);
		virtual ~OpenGLImageCube() override;

		virtual void Release() override;
		virtual void Invalidate() override;

		RendererID GetRenderingID() const { return m_RenderingID; }
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual ImageFormat GetFormat() const override { return m_Format; }

		virtual Memory::Buffer GetBuffer() const override { return m_ImageData; }

		virtual uint64_t GetImageID() const override { return m_RenderingID; }
	private:
		ImageFormat m_Format;
		std::uint32_t m_Width;
		std::uint32_t m_Height;
		Memory::Buffer m_ImageData;

		RendererID m_RenderingID = 0;
		RendererID m_SamplerRenderingID = 0;
	};
}