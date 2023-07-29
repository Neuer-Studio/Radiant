#pragma once

#include "RenderingTypes.hpp"

namespace Radiant
{
	enum class RenderingAPIType : uint32_t
	{
		None = 0,
		Vulkan = 1,
		OpenGL = 2
	};

	class RenderingAPI : public Memory::RefCounted
	{
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		// NOTE: Tempory wile we don't have a rendering scene
		virtual void DrawIndexed(uint32_t count) = 0;
		virtual void Clear() = 0;
		//==============================

		static RenderingAPIType GetAPI() { return s_RendererAPI; }
	private:
		inline static RenderingAPIType s_RendererAPI = RenderingAPIType::OpenGL;
	};
}