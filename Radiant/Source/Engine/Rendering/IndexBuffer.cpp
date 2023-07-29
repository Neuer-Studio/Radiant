#include "Rendering/IndexBuffer.hpp"
#include "Rendering/RenderingAPI.hpp"
#include "Rendering/Platform/OpenGL/OpenGLIndexBuffer.hpp"

namespace Radiant
{
	Memory::Ref<IndexBuffer> IndexBuffer::Create(void* data, uint32_t size)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None:    return nullptr;
		case RenderingAPIType::OpenGL:  return Memory::Ref<OpenGLIndexBuffer>::Create(data, size);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}

	Memory::Ref<IndexBuffer> IndexBuffer::Create(uint32_t size)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None:    return nullptr;
		case RenderingAPIType::OpenGL:  return Memory::Ref<OpenGLIndexBuffer>::Create(size);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}
}