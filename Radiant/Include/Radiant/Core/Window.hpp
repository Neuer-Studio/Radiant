#pragma once

#include "Events/Event.hpp"
#include "Events/ApplicationEvent.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/MouseEvent.hpp"

struct GLFWwindow;
namespace Radiant
{
	struct ApplicationSpecification;

	class Window : public Memory::RefCounted
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {};

		virtual GLFWwindow* GetNativeWindow() const = 0;
		virtual std::size_t GetHeight() const = 0;
		virtual std::size_t GetWidth() const = 0;

		virtual const std::string& GetTitle() const = 0;
		virtual void SetTitle(const std::string& title) = 0;

		virtual bool IsWindowMaximized() const = 0;

		virtual void OnUpdate() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		static Memory::Shared<Window> Create(const ApplicationSpecification& specification);
	};
}