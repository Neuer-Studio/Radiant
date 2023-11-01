#pragma once

#include <glad/glad.h>
#include <Radiant/Core/RadiantFormats.hpp>

namespace Radiant
{
	enum class ImageFormat;

	namespace Utils {

		inline GLenum OpenGLImageInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA8:	return GL_RGBA8;
			case ImageFormat::RGBA:		return GL_RGBA;
			case ImageFormat::RGBA16F:	return GL_RGBA16F;
			case ImageFormat::RGBA32F:	return GL_RGBA32F;

			case ImageFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			}
			RADIANT_VERIFY(false, "Unknown image format");
			return 0;
		}

		inline GLenum OpenGLFormatDataType(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA8:
			case ImageFormat::RGBA:    return GL_UNSIGNED_BYTE;
			case ImageFormat::RGBA16F:
			case ImageFormat::RGBA32F: return GL_FLOAT;
			case ImageFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			}
			RADIANT_VERIFY(false, "Unknown image format");
			return 0;
		}

		inline uint32_t GetPixelSize(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA8:		return 3;
			case ImageFormat::RGBA:		return 4;
			case ImageFormat::RGBA32F:	return 4 * 2;
			}
			RADIANT_VERIFY(false);
			return 0;
		}

		static GLenum DepthAttachmentType(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH32F:        return GL_DEPTH_ATTACHMENT;
			case ImageFormat::DEPTH24STENCIL8: return GL_DEPTH_STENCIL_ATTACHMENT;
			}
			RADIANT_VERIFY(false, "Unknown format");
			return 0;
		}

		inline uint32_t CalculateMipCount(uint32_t width, uint32_t height)
		{
			return (uint32_t)std::floor(std::log2((width < height) ? width : height)) + 1;
		}

		inline uint32_t GetImageMemorySize(ImageFormat format, uint32_t width, uint32_t height)
		{
			return width * height * GetPixelSize(format);
		}

		static bool IsDepthFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8:
			case ImageFormat::DEPTH32F:
				return true;
			}
			return false;
		}

	}
}