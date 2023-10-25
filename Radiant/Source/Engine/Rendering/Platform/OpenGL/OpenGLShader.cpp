#include <Rendering/Platform/OpenGL/OpenGLShader.hpp>

#include <Rendering/Rendering.hpp>	
#include <glm/gtc/type_ptr.hpp>

namespace Radiant
{
#define UNIFORM_LOGGING 1
#if UNIFORM_LOGGING
#define LOG_UNIFORM(...) RA_WARN(__VA_ARGS__)
#else
#define LOG_UNIFORM
#endif

#define OPENGL_SEARCH 1

	static struct UniformsSpecification
	{
		RadiantType Type;
		std::string UniformName;
	};

	namespace Utils
	{
		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;
			if (type == "compute")
				return GL_COMPUTE_SHADER;

			return GL_NONE;
		}

		static std::unordered_map<GLenum, std::string> PreProcess(const std::string& source)
		{
			std::unordered_map<GLenum, std::string> shaderSource;
			const char* tokenType = "#type";
			const std::size_t sizeTokenType = strlen(tokenType);
			std::size_t pos = source.find(tokenType, 0);

			while (pos != std::string::npos)
			{
				std::size_t eol = source.find_first_of("\r\n", pos);
				RADIANT_VERIFY(eol != std::string::npos, "Syntax error");
				std::size_t begin = pos + sizeTokenType + 1;
				std::string type = source.substr(begin, eol - begin);
				RADIANT_VERIFY(type == "vertex" || type == "fragment" || type == "pixel" || type == "compute", "Invalid shader type specified");

				std::size_t nextLinePos = source.find_first_not_of("\r\n", eol);
				pos = source.find(tokenType, nextLinePos);
				auto shaderType = ShaderTypeFromString(type);

				shaderSource[shaderType] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

				if (shaderType == GL_COMPUTE_SHADER)
				{
					RADIANT_VERIFY(false);
				}
			}

			return shaderSource;
		}

		static std::vector<UniformsSpecification> ExtractUniformNames(const std::string& shaderCode)
		{
			std::vector<UniformsSpecification> uniformSpecification;

			std::string::size_type pos = shaderCode.find("uniform");
			while (pos != std::string::npos)
			{
				std::string::size_type startPos = pos + 7; // Length of "uniform"
				std::string::size_type endPos = shaderCode.find(";", startPos);
				if (endPos != std::string::npos)
				{
					std::string uniformDeclaration = shaderCode.substr(startPos, endPos - startPos);

					uniformDeclaration = uniformDeclaration.substr(uniformDeclaration.find_first_not_of(" \t"));
					uniformDeclaration = uniformDeclaration.substr(0, uniformDeclaration.find_last_not_of(" \t") + 1);

					std::string::size_type spacePos = uniformDeclaration.find(" ");
					if (spacePos != std::string::npos)
					{
						RadiantType uniformType = OpenGLUniformTypeToRaiantUniformType(uniformDeclaration.substr(0, spacePos));

						if (uniformType != RadiantType::None)
						{
							std::string uniformName = uniformDeclaration.substr(spacePos + 1);
							uniformSpecification.push_back({ uniformType, uniformName });
						}
					}
				}

				pos = shaderCode.find("uniform", pos + 1);
			}

			return uniformSpecification;
		}

		static ShaderStructDeclaration ParseStruct(const std::string& block)
		{
			ShaderStructDeclaration result;

			size_t structStart = block.find("struct");
			size_t nameStart = block.find_first_not_of(" \t", structStart + 6);
			size_t nameEnd = block.find_first_of(" \t\n\r{", nameStart);

			result.StructName = block.substr(nameStart, nameEnd - nameStart);

			size_t braceStart = block.find('{', nameEnd);
			size_t braceEnd = block.find('}', braceStart);

			std::string fieldsBlock = block.substr(braceStart + 1, braceEnd - braceStart - 1);

			size_t fieldStart = 0;
			while (fieldStart != std::string::npos && fieldStart < fieldsBlock.size())
			{
				size_t fieldEnd = fieldsBlock.find(';', fieldStart);
				if (fieldEnd == std::string::npos)
					break;

				std::string fieldStr = fieldsBlock.substr(fieldStart, fieldEnd - fieldStart);

				fieldStr.erase(fieldStr.begin(), std::find_if(fieldStr.begin(), fieldStr.end(), [](unsigned char ch) { return !std::isspace(ch); }));
				fieldStr.erase(std::find_if(fieldStr.rbegin(), fieldStr.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), fieldStr.end());

				size_t typeEnd = fieldStr.find_last_of(" \t");
				if (typeEnd == std::string::npos)
					break;

				std::string typeStr = fieldStr.substr(0, typeEnd);

				size_t nameStart = fieldStr.find_first_not_of(" \t\r\n", typeEnd);
				if (nameStart == std::string::npos)
					break;

				std::string name = fieldStr.substr(nameStart);

				result.Fields.push_back({ OpenGLUniformTypeToRaiantUniformType(typeStr), name });

				fieldStart = fieldEnd + 1;
			}

			return result;
		}

		/*
		* std::string = Uniform Name
		* ShaderStructDeclaration = Sturct declaration (fields)
		*/
		static std::unordered_map<std::string, ShaderStructDeclaration> ExtractShaderStruct(const std::string& shaderCode)
		{
			std::unordered_map<std::string, ShaderStructDeclaration> structsMap;

			std::vector<ShaderStructDeclaration> structsDecl;

			size_t pos = 0;
			while ((pos = shaderCode.find("struct", pos)) != std::string::npos)
			{
				size_t braceEnd = shaderCode.find('}', pos);
				std::string structBlock = shaderCode.substr(pos, braceEnd - pos + 1);

				structsDecl.push_back(ParseStruct(structBlock));
				pos = braceEnd + 1;
			}

			pos = 0;
			while ((pos = shaderCode.find("uniform", pos)) != std::string::npos)
			{
				size_t nameStart = shaderCode.find_first_not_of(" \t", pos + 7);
				size_t nameEnd = shaderCode.find_first_of(" \t;", nameStart);
				std::string uniformName = shaderCode.substr(nameStart, nameEnd - nameStart);

				nameStart = nameEnd + 1;
				nameEnd = shaderCode.find(";", nameStart);

				std::string varName = shaderCode.substr(nameStart, nameEnd - nameStart);

				for (const auto& decl : structsDecl)
				{
					if (decl.StructName == uniformName)
					{
						structsMap[varName] = decl;
						break;
					}
				}

				pos = nameEnd + 1;
			}

			return structsMap;
		}
	}

	// ================================== Utils end ==========================

	void OpenGLShader::Parse()
	{
		auto& vertexSource = m_ShaderSource[GL_VERTEX_SHADER];
		auto& fragmentSource = m_ShaderSource[GL_FRAGMENT_SHADER];

		auto fragmentUniforms = Utils::ExtractUniformNames(fragmentSource);

		// Parsing Fragment shader (Sampler include)
		{
			int32_t uIndex = 0; // regular uniform
			int32_t sIndex = 0; // sampler uniform

			for (auto name : fragmentUniforms)
			{
				RadiantType Type = fragmentUniforms[uIndex].Type;
				std::string Name = fragmentUniforms[uIndex].UniformName;

				// Extract samplers
				if (Type == RadiantType::sampler1D || Type == RadiantType::sampler2D || Type == RadiantType::sampler3D)
				{
					m_SamplerUniforms.Uniforms[Name] = { Type, UniformTarget::Sampler, Name, sIndex++ };
				}

				// Extract regular uniforms
				else
					m_FragmentUniforms.Uniforms[Name] = { Type,UniformTarget::Fragment, Name, GetExternalUniformPosition(Name.c_str()) };

				uIndex++;

			}
		}

		auto vertexUniforms = Utils::ExtractUniformNames(vertexSource);

		// Parsing Vertex shader
		{
			uint32_t uIndex = 0;
			for (auto name : vertexUniforms)
			{
				RadiantType Type = vertexUniforms[uIndex].Type;
				std::string Name = vertexUniforms[uIndex].UniformName;

				m_VertexUnfiforms.Uniforms[Name] = { Type, UniformTarget::Vertex, Name, GetExternalUniformPosition(Name.c_str()) };

				uIndex++;

			}
		}

		auto fragmentUniformsStruct = Utils::ExtractShaderStruct(fragmentSource);

		// Parsing struct uniforms
		{
			for (auto& [name, structDecl] : fragmentUniformsStruct)
			{
				for (auto& field : structDecl.Fields)
				{
					std::string GLSLName = name + "." + field.Name;
					field.Position = GetExternalUniformPosition(GLSLName);
					field.GLSLName = GLSLName;

					m_FragmentUniforms.Uniforms[GLSLName] = { field.Type, UniformTarget::Fragment, GLSLName, field.Position };

				}
			}
		}

	}

	void OpenGLShader::UploadSamplerUniforms()
	{
		Memory::Shared<OpenGLShader> instance = this;
		Rendering::SubmitCommand([instance]() mutable
			{
				for (const auto& [Name, Uniform] : instance->m_SamplerUniforms.Uniforms)
				{
					glUniform1i(instance->GetExternalUniformPosition(Name.c_str()), Uniform.Position);
				}
			});
	}

	OpenGLShader::OpenGLShader(const std::filesystem::path& path)
		: m_AssetPath(path)
	{
		m_Name = Utils::FileSystem::GetFileName(path);
		Reload();
	}

	void OpenGLShader::CompileAndUploadShader()
	{
		std::vector<GLuint> shaderRendererIDs;

		GLuint program = glCreateProgram();
		for (auto& kv : m_ShaderSource)
		{
			GLenum type = kv.first;
			std::string& source = kv.second;

			GLuint shaderRendererID = glCreateShader(type);
			const GLchar* sourceCstr = (const GLchar*)source.c_str();
			glShaderSource(shaderRendererID, 1, &sourceCstr, 0);

			glCompileShader(shaderRendererID);

			GLint isCompiled = 0;
			glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderRendererID, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

				RA_ERROR("Shader compilation failed ({0}):\n{1}", m_AssetPath.string(), &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shaderRendererID);

				RADIANT_VERIFY(false, "Failed");
			}

			shaderRendererIDs.push_back(shaderRendererID);
			glAttachShader(program, shaderRendererID);
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			RADIANT_VERIFY("Shader linking failed ({0}):\n{1}", m_AssetPath.string(), &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : shaderRendererIDs)
				glDeleteShader(id);
		}

		// Always detach shaders after a successful link.
		for (auto id : shaderRendererIDs)
			glDetachShader(program, id);

		m_RenderingID = program;
	}

	std::string OpenGLShader::ReadShaderFromFile(const std::filesystem::path& filepath) const
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		RADIANT_VERIFY(in, "Could not load shader! {}", filepath.string().c_str());

		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());

		in.close();
		return result;
	}

	void OpenGLShader::Reload()
	{
		std::string source = ReadShaderFromFile(m_AssetPath);
		Load(source);
	}

	void OpenGLShader::Load(const std::string& source)
	{
		RADIANT_VERIFY(!m_RenderingID);

		m_ShaderSource = Utils::PreProcess(source);

		Rendering::SubmitCommand([=]()
			{
				CompileAndUploadShader();
				Parse();
				UploadSamplerUniforms();
			});

	}

	void OpenGLShader::Bind()
	{
		RendererID id = m_RenderingID;
		Memory::Shared<OpenGLShader> instance = this;
		Rendering::SubmitCommand([id]()
			{
				glUseProgram(id);
			});
	}

	void OpenGLShader::Unbind()
	{
		Rendering::SubmitCommand([]()
			{
				glUseProgram(0);
			});
	}

	bool OpenGLShader::HasBufferUniform(const std::string& uniformName, UniformTarget type) const
	{
		switch (type)
		{
		case UniformTarget::None: // NOTE: does not search in sampler buffer
			return (m_FragmentUniforms.Uniforms.count(uniformName) > 0) ||
				(m_VertexUnfiforms.Uniforms.count(uniformName) > 0);

		case UniformTarget::Fragment:
			return m_FragmentUniforms.Uniforms.count(uniformName) > 0();

		case UniformTarget::Vertex:
			return m_VertexUnfiforms.Uniforms.count(uniformName) > 0();

		case UniformTarget::Sampler:
			return m_SamplerUniforms.Uniforms.count(uniformName) > 0();
		}

		return false;
	}

	ShaderUniformDeclaration& OpenGLShader::GetBufferUniform(const std::string& uniformName, UniformTarget type)
	{
		switch (type)
		{
		case UniformTarget::None:
			return m_FragmentUniforms.Uniforms[0];

		case UniformTarget::Fragment:
			if (m_FragmentUniforms.Uniforms.count(uniformName) > 0)
				return m_FragmentUniforms.Uniforms.at(uniformName);
			break;

		case UniformTarget::Vertex:
			if (m_VertexUnfiforms.Uniforms.count(uniformName) > 0)
				return m_VertexUnfiforms.Uniforms.at(uniformName);
			break;

		case UniformTarget::Sampler:
			if (m_SamplerUniforms.Uniforms.count(uniformName) > 0)
				return m_SamplerUniforms.Uniforms.at(uniformName);
			break;
		}

		return m_FragmentUniforms.Uniforms[0];
	}

	//====================== Unifrom ==========================

	void OpenGLShader::UploadUniformInt(int32_t location, int value, UniformTarget type)
	{
		if (location != -1)
			glUniform1i(location, value);
		else
			LOG_UNIFORM("Uniform 'X' not found!");
	}

	void OpenGLShader::UploadUniformUint(int32_t location, int value, UniformTarget type)
	{
		if (location != -1)
			glUniform1ui(location, value);
		else
			LOG_UNIFORM("Uniform 'X' not found!");
	}

	void OpenGLShader::UploadUniformFloat(int32_t location, float value, UniformTarget type)
	{
		if (location != -1)
			glUniform1f(location, value);
		else
			LOG_UNIFORM("Uniform 'X' not found!");
	}

	void OpenGLShader::UploadUniformFloat2(int32_t location, const glm::vec2& values, UniformTarget type)
	{
		if (location != -1)
			glUniform2f(location, values.x, values.y);
		else
			LOG_UNIFORM("Uniform 'X' not found!");
	}


	void OpenGLShader::UploadUniformFloat3(int32_t location, const glm::vec3& values, UniformTarget type)
	{
		if (location != -1)
			glUniform3f(location, values.x, values.y, values.z);
		else
			LOG_UNIFORM("Uniform 'X' not found!");
	}

	void OpenGLShader::UploadUniformMat4(int32_t location, const glm::mat4& values, UniformTarget type)
	{
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(values));
		else
			LOG_UNIFORM("Uniform 'X' not found!");
	}

	int32_t OpenGLShader::GetExternalUniformPosition(const std::string& uniformName)
	{
		glUseProgram(m_RenderingID);
		return glGetUniformLocation(m_RenderingID, uniformName.c_str());
	}

	// ========================================================================

	void OpenGLShader::UpdateShaderValue(const ShaderUniformDeclaration& decl)
	{
		if (decl.Type == RadiantType::Int)
		{
			UploadUniformInt(decl.Position, *(int*)&decl.Value, decl.Target);
			return;
		}

		if (decl.Type == RadiantType::Uint)
		{
			UploadUniformUint(decl.Position, *(uint32_t*)&decl.Value, decl.Target);
			return;
		}

		if (decl.Type == RadiantType::Float)
		{
			UploadUniformFloat(decl.Position, *(float*)&decl.Value, decl.Target);
			return;
		}

		if (decl.Type == RadiantType::Float2)
		{
			UploadUniformFloat2(decl.Position, *(glm::vec2*)&decl.Value, decl.Target);
			return;
		}

		if (decl.Type == RadiantType::Float3)
		{
			UploadUniformFloat3(decl.Position, *(glm::vec3*)&decl.Value, decl.Target);
			return;
		}

		if (decl.Type == RadiantType::Mat4)
		{
			UploadUniformMat4(decl.Position, *(glm::mat4*)&decl.Value, decl.Target);
			return;
		}
	}

	int32_t OpenGLShader::GetRadiantUniformPosition(const std::string& uniformName, UniformTarget type)
	{
		auto findUniformPosition = [=](const std::string& name, UniformBuffer& buffer) -> int32_t
		{
			auto it = buffer.Uniforms.find(name);
			if (it != buffer.Uniforms.end())
			{
				int32_t position = it->second.Position;
				RADIANT_VERIFY(position >= 0);
				return position;
			}
			return -1;
		};

		switch (type)
		{
		case UniformTarget::None:
		{

			int32_t position = findUniformPosition(uniformName, m_FragmentUniforms);
			if (position >= 0)
				return position;

			position = findUniformPosition(uniformName, m_VertexUnfiforms);
			if (position >= 0)
				return position;

#ifdef OPENGL_SEARCH

			RA_ERROR("[OpenGLShader] GetRadiantUniformPosition() calls GetUniformPosition(). Incorrect position parsing!");
			return GetExternalUniformPosition(uniformName.c_str());
#else
			return -1;
#endif
		}
		case UniformTarget::Fragment:
			return findUniformPosition(uniformName, m_FragmentUniforms);

		case UniformTarget::Vertex:
			return findUniformPosition(uniformName, m_VertexUnfiforms);
		default:
			return -1;
		}
	}
}
