#include <Rendering/Shader.hpp>

#include <Radiant/Rendering/Platform/OpenGL/OpenGLShader.hpp>

namespace Radiant
{
	static std::vector<Memory::Shared<Shader>> s_AllShaders;

	Memory::Shared<Shader> Shader::Create(const std::filesystem::path& path)
	{
		Memory::Shared<Shader> shader = nullptr;
		switch (RenderingAPI::GetAPI())
		{
			case RenderingAPIType::None: return nullptr;
			case RenderingAPIType::OpenGL: shader = Memory::Shared<OpenGLShader>::Create(path);
		}
		s_AllShaders.push_back(shader);
		return shader;
	}

	void ShaderLibrary::Add(const Memory::Shared<Shader>& shader)
	{
		auto& name = shader->GetName();
		RADIANT_VERIFY(m_Shaders.find(name) == m_Shaders.end(), "");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Load(const std::filesystem::path& filepath)
	{
		auto shader = Memory::Shared<Shader>(Shader::Create(filepath));
		auto& name = shader->GetName();
		RADIANT_VERIFY(m_Shaders.find(name) == m_Shaders.end(), "");
		m_Shaders[name] = shader;
	}
	void ShaderLibrary::Load(const std::string& name, const std::filesystem::path& filepath)
	{
		RADIANT_VERIFY(m_Shaders.find(name) == m_Shaders.end(), "");
		m_Shaders[name] = Memory::Shared<Shader>(Shader::Create(filepath));
	}

	const Memory::Shared<Shader>& ShaderLibrary::Get(const std::string& name) const
	{
		RADIANT_VERIFY(m_Shaders.find(name) != m_Shaders.end(), "");
		return m_Shaders.at(name);
	}
}