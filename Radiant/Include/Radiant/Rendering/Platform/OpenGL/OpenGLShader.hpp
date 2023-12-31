#pragma once

#include <Radiant/Rendering/Shader.hpp>
#include <Radiant/Rendering/Material.hpp>

#include <glad/glad.h>

namespace Radiant
{
	class OpenGLMaterial;
	class OpenGLShader final : public Shader
	{
	public:
		OpenGLShader(const std::filesystem::path& path);
		~OpenGLShader() override {}

		virtual void Bind() override;
		virtual void Unbind() override;

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
	public:
		void UploadUniformInt(int32_t location, int value, UniformTarget type);
		void UploadUniformUint(int32_t location, int value, UniformTarget type);
		void UploadUniformBool(int32_t location, bool value, UniformTarget type);

		void UploadUniformFloat(int32_t location, float value, UniformTarget type);
		void UploadUniformFloat2(int32_t location, const glm::vec2& values, UniformTarget type);
		void UploadUniformFloat3(int32_t location, const glm::vec3& values, UniformTarget type);

		void UploadUniformMat4(int32_t location, const glm::mat4& values, UniformTarget type);
	//private:
		int32_t GetExternalUniformPosition(const std::string& uniformName);
		int32_t GetRadiantUniformPosition(const std::string& uniformName, UniformTarget type);
	public:
		virtual bool HasBufferUniform(const std::string& uniformName, UniformTarget type) const override;
		virtual ShaderUniformDeclaration& GetBufferUniform(const std::string& uniformName, UniformTarget type) override;
	private:
		bool m_IsCompute = false;
		bool m_Loaded = false;
		RendererID m_RenderingID = 0;
		std::filesystem::path m_AssetPath;
		std::string m_Name;

		std::unordered_map<GLenum, std::string> m_ShaderSource;
	private:
		UniformBuffer m_SamplerUniforms; // Sampler uniform buffer
		UniformBuffer m_FragmentUniforms; // Fragment uniform buffer
		UniformBuffer m_VertexUnfiforms; // Vertex uniform buffer
	private:
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void UploadSamplerUniforms();
	public:
		void UpdateShaderValue(const ShaderUniformDeclaration& decl);
	private:

		friend OpenGLMaterial;
	};
}