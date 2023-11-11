#pragma once

#include <string>

#include "Window.hpp"
#include "LayerStack.hpp"
#include <Radiant/Core/Timestep.hpp>
#include <Radiant/ImGui/ImGuiLayer.hpp>

namespace Radiant
{ 
	struct ApplicationSpecification
	{
		std::string Title = "Sandbox";
		uint32_t Width = 1600;
		uint32_t Height = 900;
		bool Decorated = true;
		bool Fullscreen = false;
		bool VSync = true;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate(Timestep ts) {}

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void OnEvent(Event& event);
		void RenderImGui();

		void Run();

		static Application& Get() { return *s_Instance; }
		Memory::Shared<Window> GetWindow() { return m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Memory::Shared<Window> m_Window;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		bool m_Run;

		Timestep m_Frametime;
		Timestep m_TimeStep;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	//Iml. by client
	Application* CreateApplication(int argc, char** argv);
}