#include "Rendering/VertexBuffer.hpp"
#include "Rendering/RenderingAPI.hpp"
#include "Rendering/Platform/OpenGL/OpenGLVertexBuffer.hpp"

namespace Radiant
{
	Memory::Shared<VertexBuffer> VertexBuffer::Create(const std::string& tag, Memory::Buffer buffer, VertexBufferUsage usage)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None:    return nullptr;
			case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLVertexBuffer>::Create(tag, buffer, usage);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

	Memory::Shared<VertexBuffer> VertexBuffer::Create(const std::string& tag, std::size_t size, VertexBufferUsage usage)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None:    return nullptr;
			case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLVertexBuffer>::Create(tag, size, usage);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}
}