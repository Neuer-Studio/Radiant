#pragma once

#include <Radiant/Rendering/Image.hpp>

namespace Radiant
{
	class Texture : public Memory::RefCounted
	{
	public:
		virtual ~Texture() {}

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual ImageFormat GetImageFormat() const = 0;
		virtual std::size_t GetWidth() const = 0;
		virtual std::size_t GetHeight() const = 0;
		virtual const std::filesystem::path& GetPath() const = 0;
		virtual bool Loaded() const = 0;
		virtual uint32_t GetMipLevelCount() const = 0;
	};

	class Texture2D : public Texture 
	{	
	public:
		static Memory::Shared<Texture2D> Create(const std::filesystem::path& path, bool srgb = false);
		static Memory::Shared<Texture2D> Create(ImageFormat format, uint32_t width, uint32_t height, const void* data);
		virtual Memory::Shared<Image2D> GetImage() const = 0;
	};

	class TextureCube : public Texture
	{
	public:
		static Memory::Shared<TextureCube> Create(const std::filesystem::path& path);
		static Memory::Shared<TextureCube> Create(ImageFormat format, uint32_t width, uint32_t height);
		virtual Memory::Shared<ImageCube> GetImage() const = 0;
	};
}