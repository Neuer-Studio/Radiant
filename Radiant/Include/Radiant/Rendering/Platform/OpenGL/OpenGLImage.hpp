#pragma once

#include <Radiant/Rendering/Image.hpp>
#include <Rendering/RenderingTypes.hpp>

#include <glad/glad.h>

namespace Radiant
{
	class OpenGLImage2D final : public Image2D
	{
	public:
		OpenGLImage2D(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer);
		OpenGLImage2D(ImageFormat format, std::size_t width, std::size_t height, const void* data);
		virtual ~OpenGLImage2D() override;

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

	class OpenGLImageCube final : public ImageCube
	{
	public:
		OpenGLImageCube(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer);
		OpenGLImageCube(ImageFormat format, std::size_t width, std::size_t height, const void* data);
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

	namespace Utils {

		inline GLenum OpenGLImageFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB:     return GL_RGB;
			case ImageFormat::RGBA:
			case ImageFormat::RGBA32F: return GL_RGBA;
			}
			RADIANT_VERIFY(false, "Unknown image format");
			return 0;
		}

		inline GLenum OpenGLImageInternalFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:             return GL_RGB8;
				case ImageFormat::RGBA:            return GL_RGBA8;
				case ImageFormat::RGBA32F:         return GL_RGBA32F;
			}
			RADIANT_VERIFY(false, "Unknown image format");
			return 0;
		}

		inline GLenum OpenGLFormatDataType(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB:
			case ImageFormat::RGBA:    return GL_UNSIGNED_BYTE;
			case ImageFormat::RGBA32F: return GL_FLOAT;
			}
			RADIANT_VERIFY(false, "Unknown image format");
			return 0;
		}

	}


}