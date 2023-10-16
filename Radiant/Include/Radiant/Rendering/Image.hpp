#pragma once

#include <Radiant/Core/Core.hpp>
#include <Radiant/Memory/Buffer.hpp>

namespace Radiant
{
	enum class ImageFormat
	{
		None = 0,
		RGB,
		RGBA,
		RGBA32F,
	};

	class Image : public Memory::RefCounted
	{
	public:
		virtual ~Image() {}

		virtual void Release() = 0;
		virtual void Invalidate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual ImageFormat GetFormat() const = 0;

		virtual Memory::Buffer GetBuffer() const = 0;

		virtual uint64_t GetImageID() const = 0;
	};

	class Image2D : public Image
	{
	public:
		static Memory::Shared<Image2D> Create(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer);
		static Memory::Shared<Image2D> Create(ImageFormat format, std::size_t width, std::size_t height, const void* buffer = nullptr);
	};

	class ImageCube : public Image
	{
	public:
		static Memory::Shared<ImageCube> Create(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer);
		static Memory::Shared<ImageCube> Create(ImageFormat format, std::size_t width, std::size_t height, const std::byte* buffer = nullptr);
	};

	namespace Utils
	{
		inline uint32_t GetPixelSize(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGB:		return 3;
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
