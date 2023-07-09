#include <Radiant/Rendering/Platform/OpenGL/OpenGLTexture.hpp>
#include <Rendering/Platform/OpenGL/OpenGLImage2D.hpp>
#include <Radiant/Rendering/Rendering.hpp>

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
			RADIANT_VERIFY(false);
			RA_INFO("Loading HDR texture {}, srgb = {}", Utils::FileSystem::GetFileName(path), srgb);

			float* imageData = stbi_loadf(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
			RADIANT_VERIFY(imageData);

			Memory::Buffer buffer((std::byte*)imageData, width * height * Utils::GetImageMemorySize(ImageFormat::RGBA32F, width, height));
			m_Image = Image2D::Create(ImageFormat::RGBA32F, width, height, buffer);
		}
	
		else
		{
			RA_INFO("Loading texture {}, srgb = {}", Utils::FileSystem::GetFileName(path), srgb);

			auto* imageData = stbi_load(path.string().c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
			RADIANT_VERIFY(imageData);

			ImageFormat format = srgb ? ImageFormat::RGB : ImageFormat::RGBA;
			Memory::Buffer buffer((std::byte*)imageData, width * height * Utils::GetImageMemorySize(format, width, height));
			m_Image = Image2D::Create(format, width, height, buffer);

		}

		m_Width = width;
		m_Height = height;

		m_MipCount = Utils::CalculateMipCount(width, height);

		auto& image = m_Image;
		Rendering::Submit([image]() mutable
			{
				image->Invalidate();

				auto& buffer = image->GetBuffer();
				stbi_image_free(buffer.Data);
				buffer = Memory::Buffer();
			});

	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		Memory::Ref<Image2D> image = m_Image;
		Rendering::Submit([image]() mutable {
			image->Release();
			});

	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		Memory::Ref<OpenGLImage2D> image = m_Image.As<OpenGLImage2D>();
		Rendering::Submit([slot, image]() {
			glBindTextureUnit(slot, image->GetRenderingID());
			});

	}

}