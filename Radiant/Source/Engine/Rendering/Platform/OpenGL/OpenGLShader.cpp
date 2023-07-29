#include <Rendering/Platform/OpenGL/OpenGLShader.hpp>

#include <Rendering/Rendering.hpp>	

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
		GLSLType Type;
		std::string UniformName;
	};

	namespace Utils
	{
		static GLSLType OpenGLUniformTypeToRaiantUniformType(const std::string& uniform)
		{
			if (uniform == "sampler2D")
				return GLSLType::sampler2D;
			if (uniform == "sampler3D")
				return GLSLType::sampler3D;

			if (uniform == "float")
				return GLSLType::Float;
			if (uniform == "vec2")
				return GLSLType::Float2;
			if (uniform == "vec3")
				return GLSLType::Float3;
			if (uniform == "vec4")
				return GLSLType::Float4;

			if (uniform == "mat4")
				return GLSLType::Mat4;

			return GLSLType::None;
		}


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

		static bool StartWith(const std::string& string, const std::string& start)
		{
			return string.compare(0, start.length(), start) == 0;
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
						GLSLType uniformType = OpenGLUniformTypeToRaiantUniformType(uniformDeclaration.substr(0, spacePos));

						if (uniformType != GLSLType::None)
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

		// Parsing Fragment shader
		{ 
			int32_t uIndex = 0;
			for (auto name : fragmentUniforms)
			{
				GLSLType Type = fragmentUniforms[uIndex].Type;
				std::string Name = fragmentUniforms[uIndex].UniformName;

				// Extract samplers
				if(Type == GLSLType::sampler1D || Type == GLSLType::sampler2D || Type == GLSLType::sampler3D)
				{
					m_SamplerUniforms.Uniforms[Name] = { Type, Name, uIndex };
				}

				// Extract regular uniforms
				else 
					m_FragmentUnfiforms.Uniforms[Name] = { Type, Name, GetUniformPosition(Name.c_str()) };

				uIndex++;

			}
		}

		auto vertexUniforms = Utils::ExtractUniformNames(vertexSource);

		// Parsing Vertex shader
		{
			uint32_t uIndex = 0;
			for (auto name : vertexUniforms)
			{
				GLSLType Type = vertexUniforms[uIndex].Type;
				std::string Name = vertexUniforms[uIndex].UniformName;

				m_VertexUnfiforms.Uniforms[Name] = { Type, Name, GetUniformPosition(Name.c_str()) };

				uIndex++;

			}
		}

		auto m_StructUnfiforms = Utils::ExtractShaderStruct(fragmentSource);

		// Parsing struct uniforms
		{
			for (auto& [name, structDecl] : m_StructUnfiforms)
			{
				for (auto& field : structDecl.Fields)
				{
					std::string GLSLName = name + "." + field.Name;
					field.Position = GetUniformPosition(GLSLName);
					field.GLSLName = GLSLName;

					m_FragmentStructUnfiforms.Uniforms[GLSLName] = { field.Type, GLSLName, field.Position };

				}
			}
		}
		
	}

	void OpenGLShader::UploadSamplerUniforms()
	{
		for (const auto& [Name, Uniform] : m_SamplerUniforms.Uniforms)
		{
			glUniform1i(GetUniformPosition(Name.c_str()), Uniform.Position);
		}
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
		RADIANT_VERIFY(in, "Could not load shader!");

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

		Rendering::Submit([=]()
			{
				CompileAndUploadShader();
				UploadSamplerUniforms();
				Parse();
			});

	}

	void OpenGLShader::Bind() const
	{
		RendererID id = m_RenderingID;
		Rendering::Submit([id]()
			{
				glUseProgram(id);
			});
	}

	// ========================================================

	void OpenGLShader::SetFloat(const std::string& name, float value, UniformScope type)
	{
		Rendering::Submit([=]() {
			UploadUniformFloat(name, value, type);
			});
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value, UniformScope type)
	{
		Rendering::Submit([=]() {
			UploadUniformFloat2(name, value, type);
			});
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value, UniformScope type)
	{
		Rendering::Submit([=]() {
			UploadUniformFloat3(name, value, type);
			});
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value, UniformScope type)
	{
		Rendering::Submit([=]() {
			UploadUniformMat4(name, value, type);
			});
	}

	//====================== Unifrom ==========================

	void OpenGLShader::UploadUniformInt(const std::string& name, int value, UniformScope type)
	{
		auto location = GetRadiantUniformPosition(name, type);
		if (location != -1)
			glUniform1i(location, value);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value, UniformScope type)
	{
		auto location = GetRadiantUniformPosition(name, type);
		if (location != -1)
			glUniform1f(location, value);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values, UniformScope type)
	{
		auto location = GetRadiantUniformPosition(name, type);
		if (location != -1)
			glUniform2f(location, values.x, values.y);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}


	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values, UniformScope type)
	{
		auto location = GetRadiantUniformPosition(name, type);
		if (location != -1)
			glUniform3f(location, values.x, values.y, values.z);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& values, UniformScope type)
	{
		auto location = GetRadiantUniformPosition(name, type);
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&values);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	int32_t OpenGLShader::GetUniformPosition(const std::string& uniformName)
	{
		glUseProgram(m_RenderingID);
		return glGetUniformLocation(m_RenderingID, uniformName.c_str());
	}

	// ========================================================================

	int32_t OpenGLShader::GetRadiantUniformPosition(const std::string& uniformName, UniformScope type)
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

		switch (type) {
		case UniformScope::None:
		{
			
			int32_t position = findUniformPosition(uniformName, m_FragmentUnfiforms);
			if (position >= 0)
				return position;

			position = findUniformPosition(uniformName, m_VertexUnfiforms);
			if (position >= 0)
				return position;

#ifdef OPENGL_SEARCH
			
			RA_ERROR("[OpenGLShader] GetRadiantUniformPosition() calls GetUniformPosition(). Incorrect position parsing!");
			return GetUniformPosition(uniformName.c_str());
#else
			return -1;
#endif
		}
		case UniformScope::Fragment:
			return findUniformPosition(uniformName, m_FragmentUnfiforms);

		case UniformScope::Vertex:
			return findUniformPosition(uniformName, m_VertexUnfiforms);
		case UniformScope::Struct:		
			return findUniformPosition(uniformName, m_FragmentStructUnfiforms);

		default:
			return -1;
		}
	}
}
