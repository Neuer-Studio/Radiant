#pragma once

#include <filesystem>

#include <Radiant/Rendering/RenderingAPI.hpp>
#include <glm/glm.hpp>

namespace Radiant
{
	enum class UniformTarget // NOTE: Use for get a buffer
	{
		None = 0, Fragment, Vertex, Sampler,
	};

	enum class RadiantType
	{
		None = 0, 

		sampler1D = 1, sampler2D, sampler3D, // Sampler
		Int, // Int
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

	class Material;

	class Shader : public Memory::RefCounted
	{
	public:
		virtual ~Shader() {}
		
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual const std::string& GetName() const = 0;
		virtual const std::filesystem::path& GetPath() const = 0;
		virtual RendererID GetRendererID() const = 0;
		virtual Memory::Shared<Material> GetMaterialInstance() const = 0;

		static Memory::Shared<Shader> Create(const std::filesystem::path& path);
	public:
		virtual bool HasBufferUniform(const std::string& uniformName, UniformTarget type) const = 0;
		virtual ShaderUniformDeclaration& GetBufferUniform(const std::string& uniformName, UniformTarget type) = 0;
	};


	class ShaderLibrary : public Memory::RefCounted
	{
	public:
		ShaderLibrary()
		{

		}

		~ShaderLibrary()
		{

		}

		void Add(const Memory::Shared<Shader>& shader);
		void Load(const std::string& name, const std::filesystem::path& filepath);
		void Load(const std::filesystem::path& filepath);

		const Memory::Shared<Shader>& Get(const std::string& name) const;
		std::unordered_map<std::string, Memory::Shared<Shader>>& GetShaders() { return m_Shaders; }
	private:
		std::unordered_map<std::string, Memory::Shared<Shader>> m_Shaders;
	};

	namespace Utils
	{
		static const uint32_t GetGLMDataSizeFromRadiant(RadiantType type)
		{
			switch (type)
			{
			case RadiantType::Float:
				return sizeof(float);
			case RadiantType::Float2:
				return sizeof(glm::vec2);
			case RadiantType::Float3:
				return sizeof(glm::vec3);
			case RadiantType::Mat4:
				return sizeof(glm::mat4);
			}
			RADIANT_VERIFY(false);
			return 0u;
		}

		static const RadiantType OpenGLUniformTypeToRaiantUniformType(const std::string& uniform)
		{
			if (uniform == "sampler2D")
				return RadiantType::sampler2D;
			if (uniform == "sampler3D")
				return RadiantType::sampler3D;

			if (uniform == "int")
				return RadiantType::Int;
			if (uniform == "float")
				return RadiantType::Float;
			if (uniform == "vec2")
				return RadiantType::Float2;
			if (uniform == "vec3")
				return RadiantType::Float3;
			if (uniform == "vec4")
				return RadiantType::Float4;

			if (uniform == "mat4")
				return RadiantType::Mat4;

			return RadiantType::None;
		}

	}
}