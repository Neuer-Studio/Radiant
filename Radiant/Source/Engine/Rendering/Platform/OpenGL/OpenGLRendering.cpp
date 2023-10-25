#include <Rendering/Platform/OpenGL/OpenGLRendering.hpp>

#include <glad/glad.h>

namespace Radiant {

	static void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			RA_ERROR("[OpenGL Debug HIGH] {0}", message);
			RADIANT_VERIFY(false, "GL_DEBUG_SEVERITY_HIGH");
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			RA_WARN("[OpenGL Debug MEDIUM] {0}", message);
			break;
		case GL_DEBUG_SEVERITY_LOW:
			RA_INFO("[OpenGL Debug LOW] {0}", message);
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			break;
		}
	}

	void OpenGLRendering::Init()
	{
		RA_INFO("[OpenGLRendering] Init OpenGLRendering");
		
		glDebugMessageCallback(OpenGLLogMessage, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glFrontFace(GL_CCW);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_MULTISAMPLE);

		auto& info = RenderingAPI::GetGraphicsInfo();

		info.Vendor = (const char*)glGetString(GL_VENDOR);
		info.Renderer = (const char*)glGetString(GL_RENDERER);
		info.Version = (const char*)glGetString(GL_VERSION);

		size_t pos = info.Renderer.find('/');

		if (pos != std::string::npos) 
			info.Renderer = info.Renderer.substr(0, pos);
		

		glGetIntegerv(GL_MAX_SAMPLES, &info.MaxSamples);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &info.MaxAnisotropy);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &info.MaxTextureUnits);
	}

	void OpenGLRendering::Shutdown()
	{

	}

	void OpenGLRendering::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
	{
		if (!depthTest)
			glDisable(GL_DEPTH_TEST);

		GLenum gltype = 0;
		switch (type)
		{
		case PrimitiveType::Triangles:
			gltype = GL_TRIANGLES;
			break;
		case PrimitiveType::Lines:
			gltype = GL_LINES;
			break;
		}

		glDrawElements(gltype, count, GL_UNSIGNED_INT, nullptr);

		if (!depthTest)
			glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendering::Clear(float r, float g, float b)
	{
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

}