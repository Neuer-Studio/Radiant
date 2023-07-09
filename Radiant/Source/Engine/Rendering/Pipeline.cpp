#include <Rendering/Pipeline.hpp>

#include "Rendering/RenderingAPI.hpp"
#include <Rendering/Platform/OpenGL/OpenGLPipeline.hpp>

namespace Radiant
{
	Memory::Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::None:    return nullptr;
		case RenderingAPIType::OpenGL:  return Memory::Ref<OpenGLPipeline>::Create(spec);
		}
		RADIANT_VERIFY(false, "Unknown RendererAPI");
		return nullptr;
	}
}

