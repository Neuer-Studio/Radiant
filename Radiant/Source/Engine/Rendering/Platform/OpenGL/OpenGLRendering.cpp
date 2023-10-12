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

	}

	void OpenGLRendering::Shutdown()
	{

	}

	void OpenGLRendering::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
	{
		if (!depthTest)
			glDisable(GL_DEPTH_TEST);
		else glEnable(GL_DEPTH_TEST);

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
	}

	void OpenGLRendering::Clear(float r, float g, float b)
	{
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

}