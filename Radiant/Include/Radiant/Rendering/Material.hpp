#pragma once

#include <Radiant/Rendering/Shader.hpp>
#include <Radiant/Rendering/Texture.hpp>

namespace Radiant
{
	enum class MaterialFlag
	{
		None = BIT(0),
		DepthTest = BIT(1),
	};

	class Material : public Memory::RefCounted
	{
	public:
		virtual ~Material() = default;

		virtual Memory::Ref<Shader> GetShader() = 0;
		virtual void Bind() const = 0;

		void Set(const std::string& name, const Memory::Ref<Texture>& texture);
	public:
		uint32_t GetFlags() const { return m_MaterialFlags; }
		void SetFlag(MaterialFlag flag) { m_MaterialFlags |= (uint32_t)flag; }
	public:
		static Memory::Ref<Material> Create(const Memory::Ref<Shader>& shader);
	protected:
		uint32_t m_MaterialFlags;
	};
}