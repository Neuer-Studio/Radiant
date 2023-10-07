#include "Rendering/IndexBuffer.hpp"
#include "Rendering/RenderingAPI.hpp"
#include "Rendering/Platform/OpenGL/OpenGLIndexBuffer.hpp"

namespace Radiant
{
	Memory::Shared<IndexBuffer> IndexBuffer::Create(void* data, uint32_t size)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None:    return nullptr;
		case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLIndexBuffer>::Create(data, size);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

	Memory::Shared<IndexBuffer> IndexBuffer::Create(uint32_t size)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None:    return nullptr;
		case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLIndexBuffer>::Create(size);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}
}