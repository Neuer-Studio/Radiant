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
	specification.Width = 1600;
	specification.Height = 1600;
	specification.Title = "Title";
	return new Sandbox(specification);
}