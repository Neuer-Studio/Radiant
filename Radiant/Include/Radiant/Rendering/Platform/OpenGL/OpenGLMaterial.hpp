#pragma once

#include <Radiant/Rendering/Material.hpp>

namespace Radiant
{
	struct TextureUnit
	{
		ShaderUniformDeclaration decl;
		Memory::Shared<Texture> texture;
	};

	class OpenGLMaterial final : public Material
	{
	public:
		OpenGLMaterial(const Memory::Shared<Shader>& shader, const std::string& name);
		virtual ~OpenGLMaterial() override = default;

		virtual void GetShader(const Memory::Shared<Shader>& shader) override { m_Shader = shader; }

		virtual Memory::Shared<Shader> GetShader() { return m_Shader; }
	public:
		virtual std::size_t GetOverridedValuesSize() { return m_OverrideValues.size(); }
	public:
		virtual bool SetValue(const std::string& name, bool value, UniformTarget type) override;
		virtual bool SetValue(const std::string& name, float value, UniformTarget type) override;
		virtual bool SetValue(const std::string& name, uint32_t value, UniformTarget type) override;
		virtual bool SetValue(const std::string& name, int value, UniformTarget type) override;
		virtual bool SetValue(const std::string& name, const glm::vec2& value, UniformTarget type) override;
		virtual bool SetValue(const std::string& name, const glm::vec3& value, UniformTarget type) override;
		virtual bool SetValue(const std::string& name, const glm::vec4& value, UniformTarget type) override;
		virtual bool SetValue(const std::string& name, const glm::mat4& value, UniformTarget type) override;
		virtual bool SetValue(const std::string& name, const Memory::Shared<Texture2D>& texture) override;
		virtual bool SetValue(const std::string& name, const Memory::Shared<TextureCube>& texture) override;

		virtual bool GetBool(const std::string& name, UniformTarget type) override;
		virtual bool& GetBoolRef(const std::string& name, UniformTarget type) override;

		virtual float GetFloat(const std::string& name, UniformTarget type) override;
		virtual float& GetFloatRef(const std::string& name, UniformTarget type) override;

		virtual int GetInt(const std::string& name, UniformTarget type) override;
		virtual int& GetIntRef(const std::string& name, UniformTarget type) override;

		virtual glm::vec2 GetVec2(const std::string& name, UniformTarget type) override;
		virtual glm::vec2& GetVec2Ref(const std::string& name, UniformTarget type) override;

		virtual glm::vec3 GetVec3(const std::string& name, UniformTarget type) override;
		virtual glm::vec3& GetVec3Ref(const std::string& name, UniformTarget type) override;

		virtual glm::vec4 GetVec4(const std::string& name, UniformTarget type) override;
		virtual glm::vec4& GetVec4Ref(const std::string& name, UniformTarget type) override;

		virtual glm::mat4 GetMat4(const std::string& name, UniformTarget type) override;
		virtual glm::mat4& GetMat4Ref(const std::string& name, UniformTarget type) override;

		virtual Memory::Shared<Texture2D> GetTexture2D(const std::string& name) override;

		virtual void UpdateForRendering() override;
	private:
		bool Set(const std::string& name, const std::byte* value, UniformTarget type, RadiantType uniformType);

		template<typename T>
		T Get(const std::string& name, UniformTarget type, RadiantType uniformType)
		{
			if (!m_Shader->HasBufferUniform(name, type))
			{
				RADIANT_VERIFY(false, "");
				return {};
			}

			ShaderUniformDeclaration& uniform = m_Shader->GetBufferUniform(name, type);

			RadiantType uType = uniform.Type;
			if (uniformType != uType)
			{
				RADIANT_VERIFY(false);
				return {};
			}
			return *(T*)&uniform.Value;
		}

		template<typename T>
		T& GetRef(const std::string& name, UniformTarget type, RadiantType uniformType)
		{
			if (!m_Shader->HasBufferUniform(name, type))
			{
				RADIANT_VERIFY(false, "");
				//return false;
			}

			ShaderUniformDeclaration& uniform = m_Shader->GetBufferUniform(name, type);

			RadiantType uType = uniform.Type;
			if (uniformType != uType)
			{
				RADIANT_VERIFY(false);
				//return {};
			}
			return *(T*)&uniform.Value;
		}
	private:
		std::vector<ShaderUniformDeclaration> m_OverrideValues; // Vector for overriding uniforms buffer
		std::vector<TextureUnit> m_Textures2D;
		std::vector<TextureUnit> m_TexturesCube;
		Memory::Shared<Shader> m_Shader;
		std::string m_Name;
	};
}