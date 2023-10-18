#include "WindowsWindow.hpp"

#include <Core/Events/ApplicationEvent.hpp>

namespace Radiant
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		RA_ERROR("GLFW Error: ({}: {})", error, description);
	}

	static bool s_GLFWInitialized = false;

	WindowsWindow::WindowsWindow(const WindowSpecification& specification)
		: m_Specification(specification)
	{
		RA_INFO("[Window(Windows)] Creating window {} ({}, {})", m_Specification.Title, m_Specification.Width, m_Specification.Height);

		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			RADIANT_VERIFY (success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}

		if (!m_Specification.Decorated)
		{
			// This removes titlebar on all platforms
			// and all of the native window effects on non-Windows platforms
#ifdef RADIANT_PLATFORM_WINDOWS
			glfwWindowHint(GLFW_DECORATED, false);
#else
			glfwWindowHint(GLFW_DECORATED, false);
#endif
		}

		m_Window = glfwCreateWindow((int)m_Specification.Width, (int)m_Specification.Height, m_Specification.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);

		// Create Rendering Context
		m_RendererContext = RenderingContext::Create(m_Window);
		m_RendererContext->Init();

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

				WindowResizeEvent event((unsigned int)width, (unsigned int)height);
				data.EventCallback(event);
				data.Width = width;
				data.Height = height;
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
			{
				auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
				MouseMovedEvent event((float)x, (float)y);
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event((KeyCode)key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event((KeyCode)key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event((KeyCode)key, 1);
					data.EventCallback(event);
					break;
				}
				}
			});


		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event((MouseButton)button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event((MouseButton)button);
					data.EventCallback(event);
					break;
				}
				}
			});

		// Update window size to actual size
		{
			int width, height;
			glfwGetWindowSize(m_Window, &width, &height);
			m_Data.Width = width;
			m_Data.Height = height;
		}
	}

	WindowsWindow::~WindowsWindow()
	{

	}

	const std::string& WindowsWindow::GetTitle() const
	{
		return m_Specification.Title;
	}

	void WindowsWindow::SetTitle(const std::string& title)
	{
		m_Specification.Title = title;
	}

	bool WindowsWindow::IsWindowMaximized() const
	{
		return (bool)glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED);
	}

	void WindowsWindow::OnUpdate() const
	{
		glfwPollEvents();
		m_RendererContext->SwapBuffers();
	}

}