#include <Core/Application.hpp>
#include <Rendering/Rendering.hpp>	

#include <Radiant/ImGui/ImGuiLayer.hpp>
#include <imgui/imgui.h>

namespace Radiant
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
	{
		RADIANT_VERIFY(!s_Instance, "it is not possible to create more than one instance");
		s_Instance = this;

		WindowSpecification ws;
		ws.Width = specification.WindowWidth;
		ws.Height = specification.WindowHeight;
		ws.Title = specification.Name;

		m_Window = Window::Create(ws);

		m_Window->SetEventCallback(BIND_FN(OnEvent));

		Rendering::Init();
		Rendering::ExecuteCommand();

		m_ImGuiLayer = ImGuiLayer::Create("ImGuiLayer");
		PushLayer(m_ImGuiLayer);
	}

	Application::~Application()
	{
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

		Rendering::ExecuteCommand();
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispather(event);
		dispather.Dispatch<WindowCloseEvent>(BIND_FN(OnWindowClose));
		dispather.Dispatch<WindowResizeEvent>(BIND_FN(OnWindowResize));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Run = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		int width = e.GetWidth(), height = e.GetHeight();

		return false;
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

	void Application::RenderImGui()
	{
		m_ImGuiLayer->Begin();

		for (Layer* layer : m_LayerStack)
			layer->OnImGuiRender();

		m_ImGuiLayer->End();
	}

	void Application::Run()
	{
		OnInit();
		Rendering::ExecuteCommand();
		while (m_Run)
		{
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			Rendering::ExecuteCommand();
			RenderImGui();

			m_Window->OnUpdate();
		}
	}
}