#pragma once

#include <Core/Window.hpp>

struct GLFWwindow;
namespace Radiant
{
	class RenderingContext : public Memory::RefCounted
	{
	public:
		virtual ~RenderingContext() {}

		virtual void Init() = 0;
		virtual void SwapBuffers() const = 0;

		static Memory::Shared<RenderingContext> Create(GLFWwindow* windowHandle);
	};
}