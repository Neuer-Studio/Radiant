#pragma once

#include <Radiant/Rendering/Texture.hpp>
#include <Radiant/Rendering/RenderingTypes.hpp>

namespace Radiant
{ 
	class OpenGLTexture2D final : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::filesystem::path& path, bool srgb);
		OpenGLTexture2D(ImageFormat format, uint32_t width, uint32_t height, const void* data);

		virtual ~OpenGLTexture2D() override;

		virtual void Bind(uint32_t slot) const;

		virtual Memory::Shared<Image2D> GetImage() const override { return m_Image; }
		virtual ImageFormat GetImageFormat() const { return m_Image->GetFormat(); }

		virtual std::size_t GetWidth() const { return m_Width;  }
		virtual std::size_t GetHeight() const { return m_Height; }
		virtual const std::filesystem::path& GetPath() const { return m_FilePath; }

		virtual bool Loaded() const override { return m_Loaded; }

		virtual uint32_t GetMipLevelCount() const { return m_MipCount; }
	public:
		Memory::Shared<Image2D> m_Image;
		std::filesystem::path m_FilePath;

		uint32_t m_MipCount;
		RendererID m_RenderingID = 0;
		std::size_t m_Width;
		std::size_t m_Height;
		bool m_Loaded = false;
	};

	class OpenGLTextureCube final : public TextureCube
	{
	public:
		OpenGLTextureCube(const std::filesystem::path& path);
		OpenGLTextureCube(ImageFormat format, uint32_t width, uint32_t height);
		virtual ~OpenGLTextureCube() override;

		virtual void Bind(uint32_t slot) const;

		virtual Memory::Shared<ImageCube> GetImage() const { return m_Image; }
		virtual ImageFormat GetImageFormat() const { return m_Image->GetFormat(); }

		virtual std::size_t GetWidth() const { return m_Width; }
		virtual std::size_t GetHeight() const { return m_Height; }
		virtual const std::filesystem::path& GetPath() const { return m_FilePath; }

		virtual bool Loaded() const override { return m_Loaded; }

		virtual uint32_t GetMipLevelCount() const { return m_MipCount; }
	public:
		Memory::Shared<ImageCube> m_Image;
		std::filesystem::path m_FilePath;
		uint32_t m_Width;
		uint32_t m_Height;
		ImageFormat m_Format;
		uint32_t m_MipCount;
		bool m_Loaded = false;
	};

}