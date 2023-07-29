#pragma once

#include <filesystem>

#include <Radiant/Rendering/RenderingAPI.hpp>
#include <glm/glm.hpp>

namespace Radiant
{
	enum class UniformScope
	{
		None = 0, Fragment, Vertex, Struct
	};

	enum class GLSLType
	{
		None = 0, sampler1D = 1, sampler2D, sampler3D, // Sampler
		Float, Float2, Float3, Float4, // Float 
		Mat2, Mat3, Mat4, // Mat
	};

	// Uniforms
	struct ShaderUniformDeclaration
	{
		GLSLType Type = GLSLType::None; // Uniform Type
		std::string Name = "None"; // Uniform name
		int32_t Position = -1; // Location in GLSL
	};

	struct UniformBuffer
	{
		std::unordered_map<std::string, ShaderUniformDeclaration> Uniforms;
	};
	//================================================
	
	// Struct
	struct ShaderStructDeclaration
	{
		std::string StructName = "None"; // Struct name

		struct SFields
		{
			GLSLType Type; // Filed type
			std::string Name; // Filed name
			std::string GLSLName; // Name of full field ('mystruct.field')
			int32_t Position = -1; // Location in GLSL
		};
		std::vector<SFields> Fields; // list of fields
	};

	struct StructBuffer
	{
		std::unordered_map<std::string, ShaderStructDeclaration> Uniforms;
	};

	//================================================

	class Shader : public Memory::RefCounted
	{
	public:
		virtual ~Shader() {}
		
		virtual void Bind() const = 0;

		virtual const std::string& GetName() const = 0;
		virtual const std::filesystem::path& GetPath() const = 0;
		virtual RendererID GetRendererID() const = 0;

		static Memory::Ref<Shader> Create(const std::filesystem::path& path);
	public:
		virtual void SetFloat(const std::string& name, float value, UniformScope type = UniformScope::None) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value, UniformScope type = UniformScope::None) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value, UniformScope type = UniformScope::None) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value, UniformScope type = UniformScope::None) = 0;
	};

}