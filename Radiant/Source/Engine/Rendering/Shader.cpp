#include <Rendering/Shader.hpp>

#include <Radiant/Rendering/Platform/OpenGL/OpenGLShader.hpp>

namespace Radiant
{

	Memory::Shared<Shader> Shader::Create(const std::filesystem::path& path)
	{
		Memory::Shared<Shader> shader = nullptr;
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None: return nullptr;
			case RenderingAPIType::OpenGL: shader = Memory::Shared<OpenGLShader>::Create(path);
		}
		return shader;
	}

}