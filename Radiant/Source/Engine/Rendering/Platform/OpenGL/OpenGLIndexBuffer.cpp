#include <Rendering/Platform/OpenGL/OpenGLIndexBuffer.hpp>
#include <Rendering/Rendering.hpp>

#include <glad/glad.h>

namespace Radiant
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, uint32_t size)
		: m_Size(size)
	{
		m_LocalData = Memory::Buffer::Copy(data, size);
		Memory::Shared<OpenGLIndexBuffer> instance = this;
		Rendering::SubmitCommand([instance]() mutable
			{
				glGenBuffers(1, &instance->m_RenderingID);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RenderingID);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, instance->m_LocalData.Size, instance->m_LocalData.Data, GL_STATIC_DRAW);
			}
		);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size)
		: m_Size(size)
	{
		Memory::Shared<OpenGLIndexBuffer> instance = this;
		Rendering::SubmitCommand([instance]() mutable
			{
				glGenBuffers(1, &instance->m_RenderingID);
			}
		);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		Memory::Shared<OpenGLIndexBuffer> instance = this;
		Rendering::SubmitCommand([instance]() mutable
			{
				glDeleteBuffers(1, &instance->m_RenderingID);
			}
		);
	}

	void OpenGLIndexBuffer::Bind() const {

		const Memory::Shared<const OpenGLIndexBuffer> instance = this;
		Rendering::SubmitCommand([instance]() mutable
			{
				auto id = instance->m_RenderingID;
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RenderingID);
			}
		);

	}

	void OpenGLIndexBuffer::Unbind() const {
		Rendering::SubmitCommand([]() mutable
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
			}
		);

	}

	void OpenGLIndexBuffer::SetData(void* data, uint32_t size, uint32_t offset)
	{
		m_LocalData = Memory::Buffer::Copy(data, size);
		m_Size = size;

		Memory::Shared<OpenGLIndexBuffer> instance = this;
		Rendering::SubmitCommand([instance]() mutable
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RenderingID);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, instance->m_LocalData.Size, instance->m_LocalData.Data, GL_STATIC_DRAW);
			}
		);
	}
}
