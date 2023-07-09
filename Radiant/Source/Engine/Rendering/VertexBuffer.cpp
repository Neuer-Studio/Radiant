#include "Rendering/VertexBuffer.hpp"
#include "Rendering/RenderingAPI.hpp"
#include "Rendering/Platform/OpenGL/OpenGLVertexBuffer.hpp"

namespace Radiant
{
	Memory::Ref<VertexBuffer> VertexBuffer::Create(Memory::Buffer buffer, VertexBufferUsage usage)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None:    return nullptr;
			case RenderingAPIType::OpenGL:  return Memory::Ref<OpenGLVertexBuffer>::Create(buffer, usage);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

	Memory::Ref<VertexBuffer> VertexBuffer::Create(std::size_t size, VertexBufferUsage usage)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None:    return nullptr;
			case RenderingAPIType::OpenGL:  return Memory::Ref<OpenGLVertexBuffer>::Create(size, usage);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}
}