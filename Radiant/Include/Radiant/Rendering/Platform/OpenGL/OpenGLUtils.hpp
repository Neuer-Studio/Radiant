#pragma once

#include <glad/glad.h>
#include <Radiant/Core/RadiantFormats.hpp>

namespace Radiant
{
	enum class ImageFormat;

	namespace Utils {

		inline GLenum OpenGLImageFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:		return GL_RGB8;
			case ImageFormat::RGBA:		return GL_RGBA;
			case ImageFormat::RGB16F:	return GL_RGB16F;
			case ImageFormat::RGBA32F:	return GL_RGBA32F;
			}
			RADIANT_VERIFY(false, "Unknown image format");
			return 0;
		}

		inline GLenum OpenGLFormatDataType(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:
			case ImageFormat::RGBA:    return GL_UNSIGNED_BYTE;
			case ImageFormat::RGB16F:
			case ImageFormat::RGBA32F: return GL_FLOAT;
			}
			RADIANT_VERIFY(false, "Unknown image format");
			return 0;
		}

		inline uint32_t GetPixelSize(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:		return 3;
			case ImageFormat::RGBA:		return 4;
			case ImageFormat::RGBA32F:	return 4 * 2;
			}
			RADIANT_VERIFY(false);
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
	}
}