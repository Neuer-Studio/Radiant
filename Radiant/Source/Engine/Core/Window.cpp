#include <Core/Window.hpp>

#include <Platform/Windows/WindowsWindow.hpp>
#include <Radiant/Core/Application.hpp>

namespace Radiant
{
	Memory::Shared<Window> Window::Create(const ApplicationSpecification& specification)
	{
#if defined(RADIANT_PLATFORM_WINDOWS)
		return new WindowsWindow(specification);
#else 
	#error "Current Platform doesn't supports"
#endif

	}
}