#include <Radiant/Rendering/Platform/OpenGL/OpenGLImage2D.hpp>
#include <Radiant/Rendering/Rendering.hpp>

namespace Radiant
{

	OpenGLImage2D::OpenGLImage2D(ImageFormat format, std::size_t width, std::size_t height, Memory::Buffer buffer)
		: m_Format(format), m_Width(width), m_Height(height), m_ImageData(buffer)
	{
		
	}

	OpenGLImage2D::OpenGLImage2D(ImageFormat format, std::size_t width, std::size_t height, const void *data)
		: m_Format(format), m_Width(width), m_Height(height)
	{
		if (data)
			m_ImageData = Memory::Buffer::Copy(data, Utils::GetImageMemorySize(format, width, height));
	}

	OpenGLImage2D::~OpenGLImage2D()
	{
		Release();
	}

	void OpenGLImage2D::Invalidate() // TODO: Many seetings
	{
		if (m_RenderingID)
			Release();

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RenderingID);
		GLenum internalFormat = Utils::OpenGLImageInternalFormat(m_Format);
		uint32_t mipCount = Utils::CalculateMipCount(m_Width, m_Height);
		glTextureStorage2D(m_RenderingID, mipCount, internalFormat, m_Width, m_Height);

		if (m_ImageData)
		{
			GLenum format = Utils::OpenGLImageFormat(m_Format);
			GLenum dataType = Utils::OpenGLFormatDataType(m_Format);
			glTextureSubImage2D(m_RenderingID, 0, 0, 0, m_Width, m_Height, format, dataType, m_ImageData.Data);
			glGenerateTextureMipmap(m_RenderingID); // TODO: optional
		}

		glCreateSamplers(1, &m_SamplerRenderingID);
		glSamplerParameteri(m_SamplerRenderingID, GL_TEXTURE_MIN_FILTER, m_ImageData ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glSamplerParameteri(m_SamplerRenderingID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(m_SamplerRenderingID, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glSamplerParameteri(m_SamplerRenderingID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(m_SamplerRenderingID, GL_TEXTURE_WRAP_T, GL_REPEAT);


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

}