#pragma once

#include "Rendering/RenderingContext.hpp"

struct GLFWwindow;
namespace Radiant
{
	class OpenGLContext : public RenderingContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		~OpenGLContext() override;

		void Init() override;
		void SwapBuffers() const override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}