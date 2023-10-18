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

	OpenGLVertexBuffer::OpenGLVertexBuffer(const std::string& tag, Memory::Buffer buffer, VertexBufferUsage usage)
		: m_Buffer(buffer), m_Usage(usage), m_Size(buffer.Size), m_Tag(tag)
	{
		Memory::Shared<OpenGLVertexBuffer> instance(this);
		Rendering::SubmitCommand([instance]() mutable
			{
				RA_INFO("[OpenGLVertexBuffer] OpenGLVertexBuffer::OpenGLVertexBuffer");
				glGenBuffers(1, &instance->m_RenderingID);

				glBindBuffer(GL_ARRAY_BUFFER, instance->m_RenderingID);
				glBufferData(GL_ARRAY_BUFFER, instance->m_Buffer.Size, instance->m_Buffer.Data, OpenGLUsage(instance->m_Usage));
			});
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const std::string& tag, std::size_t size, VertexBufferUsage usage)
		: m_Buffer(), m_Usage(usage), m_Size(size), m_Tag(tag)
	{
		RADIANT_VERIFY(false);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		Memory::Shared<OpenGLVertexBuffer> instance(this);
		Rendering::SubmitCommand([instance]() mutable
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
		const Memory::Shared<const OpenGLVertexBuffer> instance = this;
		auto id = this->m_RenderingID;
		Rendering::SubmitCommand([instance]() mutable
			{
				glBindBuffer(GL_ARRAY_BUFFER, instance->m_RenderingID);
			}
		);

	}

	void OpenGLVertexBuffer::Unbind() const
	{
	
		Rendering::SubmitCommand([]() mutable
			{
				glBindBuffer(GL_ARRAY_BUFFER, 0);
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