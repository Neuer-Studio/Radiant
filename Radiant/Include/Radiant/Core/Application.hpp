#pragma once

#include <string>

#include "Window.hpp"
#include "LayerStack.hpp"

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

		void Run();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		Memory::Ref<Window> m_Window;
		LayerStack m_LayerStack;

		bool m_Run;
	};


	//Iml. by client
	Application* CreateApplication(int argc, char** argv);
}