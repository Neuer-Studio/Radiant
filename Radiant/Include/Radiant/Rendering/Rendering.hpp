#pragma once

#include <Radiant/Memory/CommandBuffer.hpp>
#include <Radiant/Rendering/RenderingPass.hpp>

namespace Radiant
{
	class Rendering 
	{
	public:
		static void Init();
		static void Shutdown();

		// NOTE: Tempory wile we don't have a rendering scene
		static void DrawIndexed(std::size_t count, bool depthTest = false);
		static void Clear();
		//==============================

		static void BeginRenderingPass(const Memory::Shared<RenderingPass>& pass);
		static void EndRenderingPass();

		static void Submit(std::function<void()> func);
		static void WaitAndRender();
	private:
		static Memory::CommandBuffer& GetRenderingCommandBuffer();
	};
}