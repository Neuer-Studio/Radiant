#include <Radiant/Rendering/Platform/OpenGL/OpenGLUtils.hpp>

#include <Radiant/Rendering/Platform/OpenGL/OpenGLImage.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <Radiant/Rendering/RenderingAPI.hpp>
#include <stb_image/stb_image.h>

namespace Radiant
{
	namespace Utils
	{
		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, RendererID* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, RendererID id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}
	}

	OpenGLImage2D::OpenGLImage2D(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer, uint32_t samples)
		: m_Format(format), m_Width(width), m_Height(height), m_ImageData(buffer), m_SamplersCount(samples)
	{
		
	}

	OpenGLImage2D::OpenGLImage2D(ImageFormat format, std::size_t width, std::size_t height, const void *data, uint32_t samples)
		: m_Format(format), m_Width(width), m_Height(height), m_SamplersCount(samples)
	{
		if (data)
			m_ImageData = Memory::Buffer::Copy(data, Utils::GetImageMemorySize(format, width, height));
	}

	OpenGLImage2D::~OpenGLImage2D()
	{
		Release();
	}

	void OpenGLImage2D::Invalidate() // TODO: clean up
	{
		if (m_RenderingID)
			Release();

		bool ms = m_SamplersCount > 1;

		Utils::CreateTextures(ms, &m_RenderingID, 1);
		Utils::BindTexture(ms, m_RenderingID);

		GLenum internal = Utils::OpenGLImageInternalFormat(m_Format);
		if (ms)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_SamplersCount, internal, m_Width, m_Height, GL_FALSE);
		}

		else
		{
			// Only RGBA aaccess for now
			GLenum format = Utils::OpenGLFormatDataType(m_Format);
			if (Utils::IsDepthFormat(m_Format))
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, m_Width, m_Height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			else 
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internal, m_Width, m_Height, 0, GL_RGBA, format, m_ImageData ? m_ImageData.As<void*>() : nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			if (!Utils::IsDepthFormat(m_Format))
				glGenerateMipmap(GL_TEXTURE_2D);
		}


		Utils::BindTexture(ms, 0);

	}
	
	void OpenGLImage2D::Release()
	{
		m_ImageData.Release();
		if (m_RenderingID)
		{
			RendererID rendererID = m_RenderingID;
			Rendering::SubmitCommand([rendererID]()
				{
					glDeleteTextures(1, &rendererID);
				});
		}

	}


	/************************** Image Cube **************************/

	OpenGLImageCube::OpenGLImageCube(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer)
	{
		m_ImageData = buffer;

		m_Width = width;
		m_Height = height;
		m_Format = format;

		Invalidate();
	}

	OpenGLImageCube::OpenGLImageCube(ImageFormat format, std::size_t width, std::size_t height, const std::byte* data)
	{
		m_ImageData = Memory::Buffer::Copy(data, width * height * Utils::GetPixelSize(format));

		m_Width = width;
		m_Height = height;
		m_Format = format;

		Invalidate();

	}

	void OpenGLImageCube::Release()
	{
		m_ImageData.Release();
		if (m_RenderingID)
		{
			RendererID rendererID = m_RenderingID;
			Rendering::SubmitCommand([rendererID]()
				{
					glDeleteTextures(1, &rendererID);
				});
		}
	}

	void OpenGLImageCube::Invalidate()
	{
		if (m_RenderingID)
			Release();

		uint32_t faceWidth = m_Width / 4;
		uint32_t faceHeight = m_Height / 3;
		RADIANT_VERIFY(faceWidth == faceHeight, "Non-square faces!");

		std::array<uint8_t*, 6> faces;
		for (int i = 0; i < faces.size(); i++)
			faces[i] = new uint8_t[faceWidth * faceHeight * Utils::GetPixelSize(m_Format)];

		int faceIndex = 0;

		for (int i = 0; i < 4; i++)
		{
			for (uint32_t y = 0; y < faceHeight; y++)
			{
				int yOffset = y + faceHeight;
				for (uint32_t x = 0; x < faceWidth; x++)
				{
					int xOffset = x + i * faceWidth;
					faces[faceIndex][(x + y * faceWidth) * 3 + 0] = (uint8_t)m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = (uint8_t)m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = (uint8_t)m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
				}
			}
			faceIndex++;
		}

		for (int i = 0; i < 3; i++)
		{
			// Skip the middle one
			if (i == 1)
				continue;

			for (uint32_t y = 0; y < faceHeight; y++)
			{
				int yOffset = y + i * faceHeight;
				for (uint32_t x = 0; x < faceWidth; x++)
				{
					int xOffset = x + faceWidth;
					faces[faceIndex][(x + y * faceWidth) * 3 + 0] = (uint8_t)m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = (uint8_t)m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = (uint8_t)m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
				}
			}
			faceIndex++;
		}

		Memory::Shared<OpenGLImageCube> instance = this;
		Rendering::SubmitCommand([instance, faceWidth, faceHeight, faces]() mutable
			{
				glGenTextures(1, &instance->m_RenderingID);
				glBindTexture(GL_TEXTURE_CUBE_MAP, instance->m_RenderingID);

				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTextureParameterf(instance->m_RenderingID, GL_TEXTURE_MAX_ANISOTROPY, RenderingAPI::GetGraphicsInfo().MaxAnisotropy);

				auto format = Utils::OpenGLImageInternalFormat(ImageFormat::RGBA8);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, faces[2]);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, faces[0]);

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, faces[4]);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, faces[5]);

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, faces[1]);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, faces[3]);

				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

				glBindTexture(GL_TEXTURE_2D, 0);

				for (int i = 0; i < faces.size(); i++)
					delete[] faces[i];
			});
	}

	OpenGLImageCube::~OpenGLImageCube()
	{

	}
}