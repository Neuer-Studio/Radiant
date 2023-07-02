#pragma once

#include <string>

#include "Window.hpp"
#include "LayerStack.hpp"
#include <Radiant/ImGui/ImGuiLayer.hpp>

namespace Radiant
{ 
	struct ApplicationSpecification
	{
		std::string Name = "TheRock";
		uint32_t WindowWidth = 1600, WindowHeight = 900;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate() {}

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void OnEvent(Event& event);
		void RenderImGui();

		void Run();

		static Application& Get() { return *s_Instance; }
		Memory::Ref<Window> GetWindow() { return m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		Memory::Ref<Window> m_Window;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		bool m_Run;
	private:
		static Application* s_Instance;
	};


	//Iml. by client
	Application* CreateApplication(int argc, char** argv);
}