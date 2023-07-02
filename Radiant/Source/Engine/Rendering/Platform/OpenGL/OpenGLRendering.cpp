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
	}

	void OpenGLRendering::Shutdown()
	{

	}

	void OpenGLRendering::Draw()
	{
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void OpenGLRendering::Clear()
	{
		float r = 18.0f / 255.0f;
		float g = 113.0f / 255.0f;
		float b = 128.0f / 255.0f;
		float a = 1.0f;

		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

}