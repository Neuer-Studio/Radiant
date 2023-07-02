#include <Rendering/Platform/OpenGL/OpenGLContext.hpp>

#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace Radiant
{

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{

	}

	OpenGLContext::~OpenGLContext()
	{

	}

	void OpenGLContext::SwapBuffers() const
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void OpenGLContext::Init()
	{
		RA_INFO("[Rendering] OpenGLContext::Create");

		RADIANT_VERIFY(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Could not intialize Glad!");

		RA_INFO("OpenGL Info:");
		RA_INFO("  Vendor: {0}", (char*)glGetString(GL_VENDOR));
		RA_INFO("  Renderer: {0}", (char*)glGetString(GL_RENDERER));
		RA_INFO("  Version: {0}", (char*)glGetString(GL_VERSION));

	}
}
