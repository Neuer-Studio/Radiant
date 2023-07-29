#pragma once

#include <Radiant/Memory/CommandBuffer.hpp>

namespace Radiant
{
	class Rendering 
	{
	public:
		static void Init();

		// NOTE: Tempory wile we don't have a rendering scene
		static void DrawIndexed(std::size_t count);
		static void Clear();
		//==============================

		static void Submit(std::function<void()> func);
		static void WaitAndRender();
	private:
		static Memory::CommandBuffer& GetRenderingCommandBuffer();
	};
}