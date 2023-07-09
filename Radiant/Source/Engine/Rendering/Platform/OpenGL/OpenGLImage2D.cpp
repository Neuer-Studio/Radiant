#include <Radiant/Rendering/Platform/OpenGL/OpenGLImage2D.hpp>
#include <Radiant/Rendering/Rendering.hpp>

namespace Radiant
{

	OpenGLImage2D::OpenGLImage2D(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer)
		: m_Format(format), m_Width(width), m_Height(height), m_ImageData(buffer)
	{
		
	}

	OpenGLImage2D::~OpenGLImage2D()
	{
		Release();
	}

	void OpenGLImage2D::Invalidate() // TODO: Many seetings
	{
		if (m_RenderingID)
			Release();

		bool srgb = m_Format == ImageFormat::RGB;

		glGenTextures(1, &m_RenderingID);
		glBindTexture(GL_TEXTURE_2D, m_RenderingID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		GLenum internalFormat = Utils::OpenGLImageFormat(m_Format);
		GLenum format = Utils::OpenGLImageInternalFormat(m_Format);
		GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, m_ImageData.Data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	void OpenGLImage2D::Release()
	{
		if (m_RenderingID)
		{
			glDeleteTextures(1, &m_RenderingID);
			m_RenderingID = 0;

			m_ImageData.Release();
		}
	}

}