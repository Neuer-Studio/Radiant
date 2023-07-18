#pragma once

#include <filesystem>

#include <Radiant/Rendering/RenderingAPI.hpp>
#include <glm/glm.hpp>

namespace Radiant
{

	enum class UniformType
	{
		None = 0, sampler1D = 1, sampler2D, sampler3D, // Sampler
		Float, Float2, Float3 // Float 
	};

	// Uniforms
	struct ShaderUniformDeclaration
	{
		UniformType Type = UniformType::None;
		std::string Name = "None";
		uint32_t Position = -1;
	};

	struct UniformBuffer
	{
		std::vector<ShaderUniformDeclaration> Uniforms;
	};

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
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;
	};

}