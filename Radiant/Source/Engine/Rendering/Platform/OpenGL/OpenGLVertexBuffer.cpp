#include <Rendering/Platform/OpenGL/OpenGLVertexBuffer.hpp>
#include <Rendering/Rendering.hpp>

#include <glad/glad.h>

namespace Radiant
{
	static GLenum OpenGLUsage(VertexBufferUsage usage)
	{
		switch (usage)
		{
		case VertexBufferUsage::Static:    return GL_STATIC_DRAW;
		case VertexBufferUsage::Dynamic:   return GL_DYNAMIC_DRAW;
		}
		RADIANT_VERIFY(false, "Unknown vertex buffer usage");
		return 0;
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(Memory::Buffer buffer, VertexBufferUsage usage)
		: m_Buffer(buffer), m_Usage(usage), m_Size(buffer.Size)
	{

		Rendering::Submit([this]()
			{
				RA_INFO("[OpenGLVertexBuffer] OpenGLVertexBuffer::OpenGLVertexBuffer");
				glGenBuffers(1, &m_RenderingID);

				glBindBuffer(GL_ARRAY_BUFFER, m_RenderingID);
				glBufferData(GL_ARRAY_BUFFER, m_Buffer.Size, m_Buffer.Data, OpenGLUsage(m_Usage));

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			});
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(std::size_t size, VertexBufferUsage usage)
		: m_Buffer(), m_Usage(usage), m_Size(size)
	{
		RADIANT_VERIFY(false);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		Memory::Ref<OpenGLVertexBuffer> instance(this);
		Rendering::Submit([instance]() mutable
			{
				glDeleteBuffers(1, &instance->m_RenderingID);
			}
		);

	}

	void OpenGLVertexBuffer::SetData(Memory::Buffer buffer, std::size_t offset /*= 0*/)
	{
		RADIANT_VERIFY(false);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		RendererID id = m_RenderingID;
		Rendering::Submit([id]() mutable
			{
				glBindBuffer(GL_ARRAY_BUFFER, id);

			}
		);

	}

	std::size_t OpenGLVertexBuffer::GetCount() const
	{
		return m_Size / sizeof(std::size_t);
	}

	std::size_t OpenGLVertexBuffer::GetSize() const
	{
		return m_Size;
	}

}