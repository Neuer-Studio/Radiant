#include <Radiant/Rendering/Platform/OpenGL/OpenGLMaterial.hpp>
#include <Radiant/Rendering/Platform/OpenGL/OpenGLShader.hpp>
#include <Radiant/Rendering/Rendering.hpp>

namespace Radiant
{

	OpenGLMaterial::OpenGLMaterial(const Memory::Shared<Shader>& shader, const std::string& name)
		: m_Shader(shader), m_Name(name)
	{
	}

	bool OpenGLMaterial::SetValue(const std::string& name, bool value, UniformTarget type)
	{
		return Set(name, (std::byte*)&value, type, RadiantType::Bool);
	}

	bool OpenGLMaterial::SetValue(const std::string& name, float value, UniformTarget type)
	{
		return Set(name, (std::byte*)&value, type, RadiantType::Float);
	}

	bool OpenGLMaterial::SetValue(const std::string& name, const glm::vec2& value, UniformTarget type)
	{
		return Set(name, (std::byte*)&value, type, RadiantType::Float2);
	}

	bool OpenGLMaterial::SetValue(const std::string& name, const glm::vec3& value, UniformTarget type)
	{
		return Set(name, (std::byte*)&value, type, RadiantType::Float3);
	}

	bool OpenGLMaterial::SetValue(const std::string& name, const glm::vec4& value, UniformTarget type)
	{
		return Set(name, (std::byte*)&value, type, RadiantType::Float4);
	}

	bool OpenGLMaterial::SetValue(const std::string& name, const glm::mat4& value, UniformTarget type)
	{
		return Set(name, (std::byte*)&value, type, RadiantType::Mat4);
	}

	bool OpenGLMaterial::SetValue(const std::string& name, int value, UniformTarget type)
	{
		return Set(name, (std::byte*)&value, type, RadiantType::Int);
	}

	bool OpenGLMaterial::SetValue(const std::string& name, uint32_t value, UniformTarget type)
	{
		return Set(name, (std::byte*)&value, type, RadiantType::Uint);
	}

	bool OpenGLMaterial::SetValue(const std::string& name, const Memory::Shared<Texture2D>& texture)
	{
		if (!m_Shader->HasBufferUniform(name, UniformTarget::Sampler))
		{
			RADIANT_VERIFY(false, "");
			return false;
		}

		ShaderUniformDeclaration& uniform = m_Shader->GetBufferUniform(name, UniformTarget::Sampler);
		RadiantType uType = uniform.Type;
		if (uType != RadiantType::sampler2D) // texture == Texture2D
		{
			RADIANT_VERIFY(false);
			return false;
		}

		uint32_t i = 0;
		for (auto& decl : m_Textures2D)
		{
			if (decl.decl.Name == uniform.Name)
			{
				m_Textures2D[i] = { uniform, texture };
				return true;
			}
			i++;
		}
		m_Textures2D.push_back({ uniform, texture});
		return true;
	}

	bool OpenGLMaterial::SetValue(const std::string& name, const Memory::Shared<TextureCube>& texture)
	{
		if (!m_Shader->HasBufferUniform(name, UniformTarget::Sampler))
		{
			RADIANT_VERIFY(false, "");
			return false;
		}

		ShaderUniformDeclaration& uniform = m_Shader->GetBufferUniform(name, UniformTarget::Sampler);
		RadiantType uType = uniform.Type;
		if (uType != RadiantType::samplerCube) // texture == TextureCube
		{
			RADIANT_VERIFY(false);
			return false;
		}

		uint32_t i = 0;
		for (auto& decl : m_TexturesCube)
		{
			if (decl.decl.Name == uniform.Name)
			{
				m_TexturesCube[i] = { uniform, texture };
				return true;
			}
			i++;
		}
		m_TexturesCube.push_back({ uniform, texture });
		return true;
	}

	bool OpenGLMaterial::GetBool(const std::string& name, UniformTarget type)
	{
		return Get<bool>(name, type, RadiantType::Bool);
	}

	bool& OpenGLMaterial::GetBoolRef(const std::string& name, UniformTarget type)
	{
		return GetRef<bool>(name, type, RadiantType::Bool);
	}

	float OpenGLMaterial::GetFloat(const std::string& name, UniformTarget type)
	{
		return Get<float>(name, type, RadiantType::Float);
	}

	float& OpenGLMaterial::GetFloatRef(const std::string& name, UniformTarget type)
	{
		return GetRef<float>(name, type, RadiantType::Float);
	}

	int OpenGLMaterial::GetInt(const std::string& name, UniformTarget type)
	{
		return Get<int>(name, type, RadiantType::Int);
	}

	int& OpenGLMaterial::GetIntRef(const std::string& name, UniformTarget type)
	{
		return GetRef<int>(name, type, RadiantType::Int);
	}

	glm::vec2 OpenGLMaterial::GetVec2(const std::string& name, UniformTarget type)
	{
		return Get<glm::vec2>(name, type, RadiantType::Float2);
	}

	glm::vec2& OpenGLMaterial::GetVec2Ref(const std::string& name, UniformTarget type)
	{
		return GetRef<glm::vec2>(name, type, RadiantType::Float2);
	}

	glm::vec3 OpenGLMaterial::GetVec3(const std::string& name, UniformTarget type)
	{
		return Get<glm::vec3>(name, type, RadiantType::Float3);
	}

	glm::vec3& OpenGLMaterial::GetVec3Ref(const std::string& name, UniformTarget type)
	{
		return GetRef<glm::vec3>(name, type, RadiantType::Float3);
	}

	glm::vec4 OpenGLMaterial::GetVec4(const std::string& name, UniformTarget type)
	{
		return Get<glm::vec4>(name, type, RadiantType::Float4);
	}

	glm::vec4& OpenGLMaterial::GetVec4Ref(const std::string& name, UniformTarget type)
	{
		return GetRef<glm::vec4>(name, type, RadiantType::Float3);
	}

	glm::mat4 OpenGLMaterial::GetMat4(const std::string& name, UniformTarget type)
	{
		return Get<glm::mat4>(name, type, RadiantType::Mat4);
	}

	glm::mat4& OpenGLMaterial::GetMat4Ref(const std::string& name, UniformTarget type)
	{
		return GetRef<glm::mat4>(name, type, RadiantType::Mat4);
	}

	Memory::Shared<Texture2D> OpenGLMaterial::GetTexture2D(const std::string& name)
	{
		if (!m_Shader->HasBufferUniform(name, UniformTarget::Sampler))
			RADIANT_VERIFY(false, "");

		ShaderUniformDeclaration& uniform = m_Shader->GetBufferUniform(name, UniformTarget::Sampler);
		uint32_t position = uniform.Position;

		for (const auto& unit : m_Textures2D)
		{
			if (unit.decl.Position == position)
				return unit.texture;
		}

		RADIANT_VERIFY(false, "Texture doesnt found!");
		return {};
	}

	// ========================================================

	bool OpenGLMaterial::Set(const std::string& name, const std::byte* value, UniformTarget type, RadiantType uniformType)
	{
		if (!m_Shader->HasBufferUniform(name, type))
		{
			RADIANT_VERIFY(false, "");
			return false;
		}

		ShaderUniformDeclaration& uniform = m_Shader->GetBufferUniform(name, type);
		RadiantType uType = uniform.Type;
		if (uniformType != uType)
		{
			RADIANT_VERIFY(false);
			return false;
		}
		uniform.isChanged = true;
		uint32_t size = Utils::GetGLMDataSizeFromRadiant(uniformType);
		std::memcpy(uniform.Value, value, size);

		uint32_t i = 0;
		for (auto& decl : m_OverrideValues)
		{
			if (decl.Name == uniform.Name)
			{
				m_OverrideValues[i] = uniform;
				return true;
			}
			i++;
		}
		m_OverrideValues.push_back(uniform);
		return true;
	}

	void OpenGLMaterial::UpdateForRendering()
	{
		if (!m_OverrideValues.size()) return;

		Memory::Shared<OpenGLMaterial> instance = this;
		Rendering::SubmitCommand([instance]() mutable
			{
				glUseProgram(instance->m_Shader.As<OpenGLShader>()->GetRendererID());
				for (const auto buffer : instance->m_OverrideValues)
				{
					instance->m_Shader.As<OpenGLShader>()->UpdateShaderValue(buffer);
				}

				for (const auto& texture : instance->m_Textures2D) // Update sampler2D
				{
					auto id = texture.texture.As<Texture2D>()->GetImage()->GetImageID();
					glBindTextureUnit(texture.decl.Position,id);
				}

				for (const auto& texture : instance->m_TexturesCube) // Update samplerCube
				{
					auto id = texture.texture.As<TextureCube>()->GetImage()->GetImageID();
					glBindTextureUnit(texture.decl.Position, id);
				}
				//instance->m_OverrideValues.clear();
				//instance->m_Textures2D.clear();
			});
	}
}