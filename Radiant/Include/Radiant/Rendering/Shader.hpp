#pragma once

#include <filesystem>

#include <Radiant/Rendering/RenderingAPI.hpp>
#include <glm/glm.hpp>

namespace Radiant
{
	enum class UniformTarget // NOTE: Use for get a buffer
	{
		None = 0, Fragment, Vertex, Sampler, VertexStruct, FragmentStruct
	};

	enum class RadiantType
	{
		None = 0, sampler1D = 1, sampler2D, sampler3D, // Sampler
		Float, Float2, Float3, Float4, // Float 
		Mat2, Mat3, Mat4, // Mat
	};

	// Uniforms
	struct ShaderUniformDeclaration
	{
		// NOTE: Do we need to use ShaderType to determine the buffer type?

		RadiantType Type = RadiantType::None; // Uniform Type
		UniformTarget Target = UniformTarget::None;
		std::string Name = "None"; // Uniform name
		int32_t Position = -1; // Location in GLSL
		//NOTE: 256 is max data size in glm (glm::dvec4)
		std::byte Value[256] = { std::byte(0) }; // Value in GLSL
		bool isChanged = false;
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
			RadiantType Type; // Filed type
			std::string Name; // Filed name
			std::string GLSLName; // Name of full field ('mystruct.field')
			int32_t Position = -1; // Location in GLSL
			std::byte* Value = nullptr; // Value in GLSL
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
		
		virtual void Bind() = 0;

		virtual const std::string& GetName() const = 0;
		virtual const std::filesystem::path& GetPath() const = 0;
		virtual RendererID GetRendererID() const = 0;

		static Memory::Ref<Shader> Create(const std::filesystem::path& path);
	public:
		virtual bool SetValue(const std::string& name, const std::byte* value, UniformTarget type) = 0;
	public:
		virtual bool HasBufferUniform(const std::string& uniformName, UniformTarget type) const = 0;
		virtual ShaderUniformDeclaration& GetBufferUniform(const std::string& uniformName, UniformTarget type) = 0;
	};
}