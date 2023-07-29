#pragma once

#include <Radiant/Rendering/Shader.hpp>

#include <glad/glad.h>

namespace Radiant
{
	class OpenGLShader final : public Shader
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
		void Parse();
	private:
		void UploadUniformInt(const std::string& name, int value, UniformScope type);

		void UploadUniformFloat(const std::string& name, float value, UniformScope type);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values, UniformScope type);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values, UniformScope type);

		void UploadUniformMat4(const std::string& name, const glm::mat4& values, UniformScope type);
	private:
		int32_t GetUniformPosition(const std::string& uniformName);
		int32_t GetRadiantUniformPosition(const std::string& uniformName, UniformScope type);
	public:
		virtual void SetFloat(const std::string& name, float value, UniformScope type = UniformScope::None) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value, UniformScope type = UniformScope::None) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value, UniformScope type = UniformScope::None) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value, UniformScope type = UniformScope::None) override;
	private:
		RendererID m_RenderingID = 0;
		std::filesystem::path m_AssetPath;
		std::string m_Name;

		std::unordered_map<GLenum, std::string> m_ShaderSource;
	private:
		UniformBuffer m_SamplerUniforms;
		UniformBuffer m_FragmentUnfiforms;
		UniformBuffer m_VertexUnfiforms;

		StructBuffer m_StructUnfiforms;
		UniformBuffer m_FragmentStructUnfiforms;
		UniformBuffer m_VertexStructUnfiforms;
	private:
		void UploadSamplerUniforms();
	};
}