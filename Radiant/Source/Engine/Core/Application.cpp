#include <Core/Application.hpp>

namespace Radiant
{
	Application::Application(const ApplicationSpecification& specification)
	{
		WindowSpecification ws;
		ws.Width = specification.WindowWidth;
		ws.Height = specification.WindowHeight;
		ws.Title = specification.Name;

		m_Window = Window::Create(ws);

		m_Window->SetEventCallback(BIND_FN(OnEvent));
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispather(event);
		dispather.Dispatch<WindowCloseEvent>(BIND_FN(OnWindowClose));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Run = false;
		return true;
	}

	void Application::Run()
	{
		while (m_Run)
		{
			m_Window->OnUpdate();
		}
	}
}