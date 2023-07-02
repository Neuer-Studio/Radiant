#include <Core/Application.hpp>
#include <Rendering/Rendering.hpp>	

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

		Rendering::Init();
		Rendering::WaitAndRender();
	}

	Application::~Application()
	{
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

		Rendering::WaitAndRender();
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

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.PopLayer(layer);
		layer->OnDetach();
	}

	void Application::Run()
	{
		OnInit();
		Rendering::WaitAndRender();
		while (m_Run)
		{
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			Rendering::WaitAndRender();

			m_Window->OnUpdate();
		}
	}
}