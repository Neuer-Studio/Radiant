#include <Rendering/Shader.hpp>

#include <Radiant/Rendering/Platform/OpenGL/OpenGLShader.hpp>

namespace Radiant
{

	Memory::Ref<Shader> Shader::Create(const std::filesystem::path& path)
	{
		Memory::Ref<Shader> shader = nullptr;
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None: return nullptr;
			case RenderingAPIType::OpenGL: shader = Memory::Ref<OpenGLShader>::Create(path);
		}
		return shader;
	}

}