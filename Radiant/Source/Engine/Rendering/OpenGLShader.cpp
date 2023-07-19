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
		UniformType Type;
		std::string UniformName;
	};

	static UniformType OpenGLUniformTypeToRaiantUniformType(const std::string& uniform)
	{
		if (uniform == "sampler2D")
			return UniformType::sampler2D;
		if (uniform == "sampler3D")
			return UniformType::sampler3D;

		if (uniform == "vec2")
			return UniformType::Float2;
		if (uniform == "vec3")
			return UniformType::Float3;
		if (uniform == "vec4")
			return UniformType::Float4;

		if (uniform == "mat4")
			return UniformType::Mat4;

		return UniformType::None;
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

	std::vector<UniformsSpecification> ExtractUniformNames(const std::string& shaderCode)
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
					UniformType uniformType = OpenGLUniformTypeToRaiantUniformType(uniformDeclaration.substr(0, spacePos));
					
					std::string uniformName = uniformDeclaration.substr(spacePos + 1);
					uniformSpecification.push_back({uniformType, uniformName});
				}
			}

			pos = shaderCode.find("uniform", pos + 1);
		}

		return uniformSpecification;
	}


	void OpenGLShader::Parse()
	{
		auto& vertexSource = m_ShaderSource[GL_VERTEX_SHADER];
		auto& fragmentSource = m_ShaderSource[GL_FRAGMENT_SHADER];

		auto fragmentUniforms = ExtractUniformNames(fragmentSource);

		// Parsing Fragment shader
		{ 
			uint32_t uIndex = 0;
			for (auto name : fragmentUniforms)
			{
				UniformType Type = fragmentUniforms[uIndex].Type;
				std::string Name = fragmentUniforms[uIndex].UniformName;

				// Extract samplers
				if(Type == UniformType::sampler1D || Type == UniformType::sampler2D || Type == UniformType::sampler3D)
				{
					m_SamplerUniforms.Uniforms[Name] = { Type, Name, uIndex };
				}

				// Extract regular uniforms
				else 
					m_FragmentUnfiforms.Uniforms[Name] = { Type, Name, GetUniformPosition(Name.c_str()) };

				uIndex++;

			}
		}

		auto vertexUniforms = ExtractUniformNames(vertexSource);

		// Parsing Vertex shader
		{
			uint32_t uIndex = 0;
			for (auto name : vertexUniforms)
			{
				UniformType Type = vertexUniforms[uIndex].Type;
				std::string Name = vertexUniforms[uIndex].UniformName;

				m_VertexUnfiforms.Uniforms[Name] = { Type, Name, GetUniformPosition(Name.c_str()) };

				uIndex++;

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

		m_ShaderSource = PreProcess(source);

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

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		Rendering::Submit([=]() {
			UploadUniformFloat3(name, value);
			});
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		Rendering::Submit([=]() {
			UploadUniformMat4(name, value);
			});
	}

	//====================== Unifrom ==========================

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		auto location = GetRadiantUniformPosition(name);
		if (location != -1)
			glUniform1i(location, value);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		auto location = GetRadiantUniformPosition(name);
		if (location != -1)
			glUniform1f(location, value);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		auto location = GetRadiantUniformPosition(name);
		if (location != -1)
			glUniform2f(location, values.x, values.y);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}


	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		auto location = GetRadiantUniformPosition(name);
		if (location != -1)
			glUniform3f(location, values.x, values.y, values.z);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& values)
	{
		auto location = GetRadiantUniformPosition(name);
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, (const float*)&values);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	uint32_t OpenGLShader::GetUniformPosition(const std::string& uniformName)
	{
		glUseProgram(m_RenderingID);
		return glGetUniformLocation(m_RenderingID, uniformName.c_str());
	}

	uint32_t OpenGLShader::GetRadiantUniformPosition(const std::string& uniformName, ShaderType type)
	{
		// If the type is 'None', we should search in all shaders.
		if (type == ShaderType::None)
		{
			auto it = m_FragmentUnfiforms.Uniforms.find(uniformName);

			if (it != m_FragmentUnfiforms.Uniforms.end())
			{
				uint32_t position = it->second.Position;
				RADIANT_VERIFY(position >= 0);
				return position;
			}
			else
			{
				it = m_VertexUnfiforms.Uniforms.find(uniformName);
				if (it != m_VertexUnfiforms.Uniforms.end())
				{
					uint32_t position = it->second.Position;
					RADIANT_VERIFY(position >= 0);
					return position;
				}
				else
				{
#ifdef OPENGL_SEARCH
					RA_ERROR("[OpenGLShader] GetRadiantUniformPosition() calls GetUniformPosition(). Incorrect position parsing!");
					return GetUniformPosition(uniformName.c_str());
#else
					return -1;
#endif
				}
			}
		}

		// If the type is 'Fragment', we should ONLY look for it in the fragment shader.
		else if (type == ShaderType::Fragment)
		{
			auto it = m_FragmentUnfiforms.Uniforms.find(uniformName);

			if (it != m_FragmentUnfiforms.Uniforms.end())
			{
				uint32_t position = it->second.Position;
				RADIANT_VERIFY(position >= 0);
				return position;
			}
			else
			{
#ifdef OPENGL_SEARCH
				RA_ERROR("[OpenGLShader] GetRadiantUniformPosition() calls GetUniformPosition(). Incorrect position parsing!");
				return GetUniformPosition(uniformName.c_str());
#else
				return -1;
#endif
			}
		}

		// If the type is 'Vertex', we should ONLY look for it in the fragment shader.
		else if (type == ShaderType::Vertex)
		{
			auto it = m_VertexUnfiforms.Uniforms.find(uniformName);

			if (it != m_VertexUnfiforms.Uniforms.end())
			{
				uint32_t position = it->second.Position;
				RADIANT_VERIFY(position >= 0);
				return position;
			}
			else
			{
#ifdef OPENGL_SEARCH
				RA_ERROR("[OpenGLShader] GetRadiantUniformPosition() calls GetUniformPosition(). Incorrect position parsing!");
				return GetUniformPosition(uniformName.c_str());
#else
				return -1;
#endif
			}
		}

		return -1;
	}
}