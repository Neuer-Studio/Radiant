#include "Radiant/Core/Logger.hpp"
#include <Radiant/Core/Core.hpp>
#include <Radiant/Core/Application.hpp>
#include <Radiant/EntryPoint.hpp>

#include "EditorLayer.hpp"

class Sandbox : public Radiant::Application
{
public:
	Sandbox(const Radiant::ApplicationSpecification& props)
		: Radiant::Application(props)
	{

	}

	void OnInit() override
	{
		PushLayer(new Radiant::EditorLayer());
	}
};

Radiant::Application* Radiant::CreateApplication(int argc, char** argv)
{
	Radiant::ApplicationSpecification specification;
	specification.WindowHeight = 1600;
	specification.WindowWidth = 1600;
	specification.Name = "Title";
	return new Sandbox(specification);
}