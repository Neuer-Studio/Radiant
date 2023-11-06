#include <Radiant/Rendering/Platform/OpenGL/OpenGLTexture.hpp>
#include <Rendering/Platform/OpenGL/OpenGLImage.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <Radiant/Rendering/Platform/OpenGL/OpenGLUtils.hpp>

#include <stb_image/stb_image.h>
#include <glad/glad.h>

namespace Radiant
{
	OpenGLTexture2D::OpenGLTexture2D(const std::filesystem::path& path, bool srgb)
		: m_FilePath(path)
	{
		RADIANT_VERIFY(Utils::FileSystem::Exists(path), "File doesn't exist!");

		int width, height, channels;
		if (stbi_is_hdr(path.string().c_str()))
		{
			RA_INFO("Loading HDR texture {}, srgb = {}, hdr = {}", Utils::FileSystem::GetFileName(path), srgb, true);

			float* imageData = stbi_loadf(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			RADIANT_VERIFY(imageData);

			Memory::Buffer buffer((std::byte*)imageData, width * height * Utils::GetImageMemorySize(ImageFormat::RGBA32F, width, height));
			m_Image = Image2D::Create(ImageFormat::RGBA16F, width, height, buffer);
		}
	
		else
		{
			RA_INFO("Loading texture {}, srgb = {}", Utils::FileSystem::GetFileName(path), srgb);

			auto* imageData = stbi_load(path.string().c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
			RADIANT_VERIFY(imageData);

			ImageFormat format = srgb ? ImageFormat::SRGB8 : ImageFormat::RGBA; // TODO: Should be rgba16f
			Memory::Buffer buffer((std::byte*)imageData, width * height * Utils::GetImageMemorySize(format, width, height));
			m_Image = Image2D::Create(format, width, height, buffer);

		}

		m_Width = width;
		m_Height = height;

		m_MipCount = Utils::CalculateMipCount(width, height);
		m_Loaded = true;
		auto& image = m_Image;
		Rendering::SubmitCommand([image]() mutable
			{
				image->Invalidate();
				stbi_image_free(image->GetBuffer().Data);
			});

	}

	OpenGLTexture2D::OpenGLTexture2D(ImageFormat format, uint32_t width, uint32_t height, const void* data)
	{
		m_Image = Image2D::Create(format, width, height, data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		Memory::Shared<Image2D> image = m_Image;
		Rendering::SubmitCommand([image]() mutable {
			image->Release();
			});

	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		Memory::Shared<OpenGLImage2D> image = m_Image.As<OpenGLImage2D>();
		Rendering::SubmitCommand([slot, image]() 
			{
				glBindTextureUnit(slot, image->GetRenderingID());
			});

	}

	/*********************** Texture Cube ***********************/

	OpenGLTextureCube::OpenGLTextureCube(const std::filesystem::path& path)
		: m_FilePath(path)
	{
		RA_INFO("Loading skylight texture {}, srgb = {}", Utils::FileSystem::GetFileName(path));
		
		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		const std::byte* imageData = (std::byte*)stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb);
		RADIANT_VERIFY(imageData);

		m_Width = width;
		m_Height = height;
		m_Format = ImageFormat::RGBA8;
		m_MipCount = Utils::CalculateMipCount(width, height);

		m_Image = ImageCube::Create(ImageFormat::RGBA8, width, height, imageData);
		stbi_image_free(m_Image->GetBuffer().Data);

		m_Loaded = true;
	}

	OpenGLTextureCube::OpenGLTextureCube(ImageFormat format, uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;
		m_MipCount = Utils::CalculateMipCount(width, height);

		m_Image = ImageCube::Create(format, width, height);
		
		m_Loaded = true;
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		Memory::Shared<Image2D> image = m_Image;
		Rendering::SubmitCommand([image]() mutable {
			});

	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		Memory::Shared<OpenGLImageCube> image = m_Image.As<OpenGLImageCube>();
		Rendering::SubmitCommand([slot, image]() {
			glBindTextureUnit(slot, image->GetRenderingID());
			});

	}

}