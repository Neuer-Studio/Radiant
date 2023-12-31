#include <Radiant/Rendering/Material.hpp>
#include <Radiant/Rendering/Platform/OpenGL/OpenGLMaterial.hpp>

namespace Radiant
{
	Memory::Shared<Material> Material::Create(const Memory::Shared<Shader>& shader, const std::string& name)
	{
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None:    return nullptr;
			case RenderingAPIType::OpenGL:  return Memory::Shared<OpenGLMaterial>::Create(shader, name);
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}
}