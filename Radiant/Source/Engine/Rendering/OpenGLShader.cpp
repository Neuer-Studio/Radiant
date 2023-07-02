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

		CompileAndUploadShader();
	}

	void OpenGLShader::Bind() const
	{
		RendererID id = m_RenderingID;
		Rendering::Submit([id]()
			{
				glUseProgram(id);
			});
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		Rendering::Submit([=]() {
			UploadUniformFloat3(name, value);
			});
	}

	//====================== Unifrom ==========================

	static void UploadUniformFloat(uint32_t location, float value)
	{
		glUniform1f(location, value);
	}

	static void UploadUniformFloat2(uint32_t location, const glm::vec2& value)
	{
		glUniform2f(location, value.x, value.y);
	}

	static void UploadUniformFloat3(uint32_t location, const glm::vec3& value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		glUseProgram(m_RenderingID);
		auto location = glGetUniformLocation(m_RenderingID, name.c_str());
		if (location != -1)
			glUniform1f(location, value);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		glUseProgram(m_RenderingID);
		auto location = glGetUniformLocation(m_RenderingID, name.c_str());
		if (location != -1)
			glUniform2f(location, values.x, values.y);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}


	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		glUseProgram(m_RenderingID);
		auto location = glGetUniformLocation(m_RenderingID, name.c_str());
		if (location != -1)
			glUniform3f(location, values.x, values.y, values.z);
		else
			LOG_UNIFORM("Uniform '{0}' not found!", name);
	}

}