#include <Rendering/Rendering.hpp>
#include <Rendering/Platform/OpenGL/OpenGLRendering.hpp>

namespace Radiant
{
	static Memory::CommandBuffer* s_CommandBuffer = nullptr;
	static RenderingAPI* s_RendererAPI = nullptr;

	static RenderingAPI* InitRenderingAPI()
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::OpenGL: return new OpenGLRendering();
		}
		RADIANT_VERIFY(false, "Unknown RendererAPI");
		return nullptr;
	}


	void Rendering::Init()
	{
		RA_INFO("[Rendering] Init Rendering");
		s_RendererAPI = InitRenderingAPI();
		s_CommandBuffer = new Memory::CommandBuffer();

		s_RendererAPI->Init();
	}

	void Rendering::Draw()
	{
		s_RendererAPI->Draw();
	}

	void Rendering::Clear()
	{
		s_RendererAPI->Clear();
	}

	void Rendering::Submit(std::function<void()> func)
	{
		s_CommandBuffer->AddCommand(func);
	}

	void Rendering::WaitAndRender()
	{
		s_CommandBuffer->Execute();
	}

	Memory::CommandBuffer& GetRenderingCommandBuffer()
	{
		return *s_CommandBuffer;
	}
}