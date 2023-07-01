#pragma once

#include <Windows.h>

#include <Core/Window.hpp>
#include <GLFW/glfw3.h>

namespace Radiant
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowSpecification& specification);
		~WindowsWindow() override;

		virtual const std::string& GetTitle() const override;
		virtual void SetTitle(const std::string& title) override;

		virtual void OnUpdate() const override;

		virtual void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; };
	private:
		WindowSpecification m_Specification;
		struct WindowData
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;

			bool VSync = false;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		GLFWwindow* m_Window;
	};
}