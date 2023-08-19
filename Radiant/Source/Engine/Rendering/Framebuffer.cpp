#include <Radiant/Rendering/RenderingAPI.hpp>
#include <Radiant/Rendering/Framebuffer.hpp>
#include <Radiant/Rendering/Platform/OpenGL/OpenGLFramebuffer.hpp>

namespace Radiant
{
	Memory::Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		Memory::Ref<Framebuffer> result = nullptr;

		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None:		return nullptr;
		case RenderingAPIType::OpenGL:	result = new OpenGLFramebuffer(spec);
		}
		//FramebufferPool::Add(result);
		return result;
	}

}