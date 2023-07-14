#pragma once

#include <filesystem>

#include <Radiant/Rendering/RenderingAPI.hpp>

namespace Radiant
{

	enum class SamplerUniformType
	{
		None = 0, sampler1D = 1, sampler2D, sampler3D
	};

	// Uniforms
	struct ShaderUniformDeclaration
	{
		std::string Name;
		std::size_t Position;
	};

	struct UniformBuffer
	{
		std::vector<ShaderUniformDeclaration> Uniforms;
	};

	// Sampler Uniforms
	struct ShaderSamplerUniformDeclaration
	{
		std::string Name;
		std::size_t Position;
	};

	struct SamplerUniformBuffer
	{
		std::vector<ShaderSamplerUniformDeclaration> Uniforms;
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
	};

}