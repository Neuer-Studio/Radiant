#include <Rendering/Rendering.hpp>
#include <Rendering/Platform/OpenGL/OpenGLRendering.hpp>

namespace Radiant
{
	struct RenderingData
	{
		Memory::Shared<RenderingPass> ActiveRenderingPass;
	};

	static RenderingData* s_Data = nullptr;

	static Memory::CommandBuffer* s_CommandBuffer = nullptr;
	static RenderingAPI* s_RendererAPI = nullptr;

	static RenderingAPI* InitRenderingAPI()
	{
		switch (RenderingAPI::GetAPI())
		{
		case RenderingAPIType::OpenGL: return new OpenGLRendering();
		}
		RADIANT_VERIFY(false, "Unknown RenderingAPI");
		return nullptr;
	}


	void Rendering::Init()
	{
		RA_INFO("[Rendering] Init Rendering");
		s_RendererAPI = InitRenderingAPI();
		s_CommandBuffer = new Memory::CommandBuffer();

		s_RendererAPI->Init();
		s_Data = new RenderingData();
	}

	void Rendering::BeginRenderingPass(const Memory::Shared<RenderingPass>& pass)
	{
		RADIANT_VERIFY(pass, "Render pass cannot be null!");
		s_Data->ActiveRenderingPass = pass;

		pass->GetSpecification().TargetFramebuffer->Bind();

	}

	void Rendering::EndRenderingPass()
	{
		RADIANT_VERIFY(s_Data->ActiveRenderingPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");

		s_Data->ActiveRenderingPass->GetSpecification().TargetFramebuffer->Unbind();
		s_Data->ActiveRenderingPass = nullptr;
	}

	void Rendering::DrawIndexed(std::size_t count, bool depthTest)
	{
		Rendering::Submit([=]()
			{
				s_RendererAPI->DrawIndexed(count, depthTest);
			});
	}

	void Rendering::Clear()
	{
		Rendering::Submit([=]()
			{
				s_RendererAPI->Clear();
			});
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