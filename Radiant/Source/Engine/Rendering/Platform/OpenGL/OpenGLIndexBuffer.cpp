#include <Rendering/Platform/OpenGL/OpenGLIndexBuffer.hpp>
#include <Rendering/Rendering.hpp>

#include <glad/glad.h>

namespace Radiant
{

	OpenGLIndexBuffer::OpenGLIndexBuffer(Memory::Buffer data)
		: m_Buffer(data), m_Size(data.Size)
	{
		Memory::Ref<OpenGLIndexBuffer> instance(this);
		Rendering::Submit([instance]() mutable
			{
				glGenBuffers(1, &instance->m_RenderingID);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RenderingID);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, instance->m_Buffer.Size, instance->m_Buffer.Data, GL_STATIC_DRAW);
			}
		);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(std::size_t size)
	{
		RADIANT_VERIFY(false);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		Memory::Ref<OpenGLIndexBuffer> instance (this);
		Rendering::Submit([instance]() mutable
			{
				glDeleteBuffers(1, &instance->m_RenderingID);
			}
		);
	}

	void OpenGLIndexBuffer::SetData(Memory::Buffer buffer, std::size_t offset /*= 0*/)
	{
		RADIANT_VERIFY(false);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		RendererID id = m_RenderingID;
		Rendering::Submit([id]() mutable
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			}
		);
	}

	std::size_t OpenGLIndexBuffer::GetCount() const
	{
		return m_Size / sizeof(std::size_t);
	}

	std::size_t OpenGLIndexBuffer::GetSize() const
	{
		return m_Size;
	}

}
