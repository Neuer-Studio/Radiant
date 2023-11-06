#pragma once

#include <Radiant/Core/Core.hpp>
#include <Radiant/Memory/Buffer.hpp>
#include <Radiant/Core/RadiantFormats.hpp>

namespace Radiant
{
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
		static Memory::Shared<Image2D> Create(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer, uint32_t samples = 1);
		static Memory::Shared<Image2D> Create(ImageFormat format, std::size_t width, std::size_t height, const void* buffer = nullptr, uint32_t samples = 1);
	};

	class ImageCube : public Image
	{
	public:
		static Memory::Shared<ImageCube> Create(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer);
		static Memory::Shared<ImageCube> Create(ImageFormat format, std::size_t width, std::size_t height, const std::byte* buffer = nullptr);
	};
}
