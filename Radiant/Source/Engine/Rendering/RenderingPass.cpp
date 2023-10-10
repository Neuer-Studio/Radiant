#include <Radiant/Rendering/RenderingPass.hpp>

#include 

namespace RockEngine {

	Memory::Shared<RenderingPass> RenderingPass::Create(const RenderingPassSpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::OpenGL:  return Memory::Shared<RenderingPass>::Create(spec);
		}

		RE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}