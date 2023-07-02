#pragma once

#include <Radiant/Rendering/Shader.hpp>

#include <glad/glad.h>

namespace Radiant
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::filesystem::path& path);
		~OpenGLShader() override {}

		virtual void Bind() const;

		virtual const std::string& GetName() const override { return m_Name; }
		virtual const std::filesystem::path& GetPath() const override { return m_AssetPath; }
		virtual RendererID GetRendererID() const override { return m_RenderingID; };
	private:
		std::string ReadShaderFromFile(const std::filesystem::path& filepath) const;
		void Reload();
		void Load(const std::string& source);
		void CompileAndUploadShader();
	private:
		RendererID m_RenderingID = 0;
		std::filesystem::path m_AssetPath;
		std::string m_Name;

		std::unordered_map<GLenum, std::string> m_ShaderSource;
	};
}