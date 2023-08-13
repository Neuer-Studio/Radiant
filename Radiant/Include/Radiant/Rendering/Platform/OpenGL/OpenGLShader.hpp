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

		virtual void Bind();

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
		void UploadUniformInt(int32_t location, int value, UniformTarget type);

		void UploadUniformFloat(int32_t location, float value, UniformTarget type);
		void UploadUniformFloat2(int32_t location, const glm::vec2& values, UniformTarget type);
		void UploadUniformFloat3(int32_t location, const glm::vec3& values, UniformTarget type);

		void UploadUniformMat4(int32_t location, const glm::mat4& values, UniformTarget type);
	private:
		int32_t GetExternalUniformPosition(const std::string& uniformName);
		int32_t GetRadiantUniformPosition(const std::string& uniformName, UniformTarget type);
		void UpdateValues();
	public:
		virtual bool SetValue(const std::string& name, const std::byte* value, UniformTarget type) override;
	public:
		virtual bool HasBufferUniform(const std::string& uniformName, UniformTarget type) const override;
		virtual ShaderUniformDeclaration& GetBufferUniform(const std::string& uniformName, UniformTarget type) override;
	private:
		RendererID m_RenderingID = 0;
		std::filesystem::path m_AssetPath;
		std::string m_Name;

		std::unordered_map<GLenum, std::string> m_ShaderSource;
	private:
		UniformBuffer m_SamplerUniforms; // Sampler uniform buffer
		UniformBuffer m_FragmentUniforms; // Fragment uniform buffer
		UniformBuffer m_VertexUnfiforms; // Vertex uniform buffer

		StructBuffer m_StructUnfiforms; // Struct uniform buffer
		UniformBuffer m_FragmentStructUnfiforms; // Struct Fragment uniform buffer
		UniformBuffer m_VertexStructUnfiforms; // Struct Vertex uniform buffer

		std::vector<ShaderUniformDeclaration> m_OverrideValues; // Vector for overriding uniforms buffer
	private:
		void UploadSamplerUniforms();
	public:
		void UpdateGLMValues(const ShaderUniformDeclaration& decl);
	};
}