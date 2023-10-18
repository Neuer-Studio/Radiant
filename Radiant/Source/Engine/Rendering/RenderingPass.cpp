#include <Radiant/Rendering/RenderingPass.hpp>
#include <Radiant/Rendering/RenderingAPI.hpp>

#include <Radiant/Rendering/Platform/OpenGL/OpenGLRenderingPass.hpp>

namespace Radiant {

	Memory::Shared<RenderingPass> RenderingPass::Create(const RenderingPassSpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLRenderingPass>::Create(spec);
		}

		RADIANT_VERIFY(false, "Unknown RendererAPI!");
		return nullptr;
	}

}