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

		virtual Memory::Shared<Shader> GetShader() = 0;
		virtual void GetShader(const Memory::Shared<Shader>& shader) = 0;
	public:
		virtual std::size_t GetOverridedValuesSize() = 0;
	public:
		virtual bool SetValue(const std::string& name, float value, UniformTarget type) = 0;
		virtual bool SetValue(const std::string& name, uint32_t value, UniformTarget type) = 0;
		virtual bool SetValue(const std::string& name, int value, UniformTarget type) = 0;
		virtual bool SetValue(const std::string& name, const glm::vec2& value, UniformTarget type) = 0;
		virtual bool SetValue(const std::string& name, const glm::vec3& value, UniformTarget type) = 0;
		virtual bool SetValue(const std::string& name, const glm::vec4& value, UniformTarget type) = 0;
		virtual bool SetValue(const std::string& name, const glm::mat4& value, UniformTarget type) = 0;
		virtual bool SetValue(const std::string& name, const Memory::Shared<Texture2D>& texture) = 0;
		virtual bool SetValue(const std::string& name, const Memory::Shared<TextureCube>& texture) = 0;

		virtual float GetFloat(const std::string& name, UniformTarget type) = 0;
		virtual float& GetFloatRef(const std::string& name, UniformTarget type) = 0;

		virtual int GetInt(const std::string& name, UniformTarget type) = 0;
		virtual int& GetIntRef(const std::string& name, UniformTarget type) = 0;

		virtual glm::vec2 GetVec2(const std::string& name, UniformTarget type) = 0;
		virtual glm::vec2& GetVec2Ref(const std::string& name, UniformTarget type) = 0;

		virtual glm::vec3 GetVec3(const std::string& name, UniformTarget type) = 0;
		virtual glm::vec3& GetVec3Ref(const std::string& name, UniformTarget type) = 0;

		virtual glm::vec4 GetVec4(const std::string& name, UniformTarget type) = 0;
		virtual glm::vec4& GetVec4Ref(const std::string& name, UniformTarget type) = 0;
		
		virtual glm::mat4 GetMat4(const std::string& name, UniformTarget type) = 0;
		virtual glm::mat4& GetMat4Ref(const std::string& name, UniformTarget type) = 0;

		virtual Memory::Shared<Texture2D> GetTexture2D(const std::string& name) = 0;

		virtual void UpdateForRendering() = 0; // NOTE(Danya): Does we need this function?
	public:
		uint32_t GetFlags() const { return m_MaterialFlags; }
		void SetFlag(MaterialFlag flag) { m_MaterialFlags |= (uint32_t)flag; }
	public:
		static Memory::Shared<Material> Create(const Memory::Shared<Shader>& shader);
	protected:
		uint32_t m_MaterialFlags;
	};
}