#include <Rendering/Rendering.hpp>
#include <Rendering/Platform/OpenGL/OpenGLRendering.hpp>

#include <Rendering/VertexBuffer.hpp>
#include <Rendering/IndexBuffer.hpp>
#include <Rendering/Pipeline.hpp>

namespace Radiant
{
	struct RenderingData
	{
		Memory::CommandBuffer* s_CommandBuffer;
		RenderingAPI* s_RendererAPI;

		Memory::Shared<RenderingPass> ActiveRenderingPass;

		Memory::Shared<VertexBuffer> m_FullscreenQuadVertexBuffer;
		Memory::Shared<IndexBuffer> m_FullscreenQuadIndexBuffer;
		Memory::Shared<Pipeline> m_FullscreenQuadPipeline;
	};

	static RenderingData* s_Data = nullptr;

	namespace
	{
		static void DrawIndexed(std::size_t count, bool depthTest)
		{
			Rendering::SubmitCommand([=]()
				{
					s_Data->s_RendererAPI->DrawIndexed(count, depthTest);
				});
		}

		static void Clear()
		{
			Rendering::SubmitCommand([=]()
				{
					s_Data->s_RendererAPI->Clear();
				});
		}
	}

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
		s_Data->s_RendererAPI = InitRenderingAPI();
		s_Data->s_CommandBuffer = new Memory::CommandBuffer();

		s_Data->s_RendererAPI->Init();
		s_Data = new RenderingData();

		// NOTE(Danya): Create fullscreen quad
		float x = -1;
		float y = -1;
		float width = 2, height = 2;
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		QuadVertex* data = new QuadVertex[4];

		data[0].Position = glm::vec3(x, y, 0.1f);
		data[0].TexCoord = glm::vec2(0, 0);

		data[1].Position = glm::vec3(x + width, y, 0.1f);
		data[1].TexCoord = glm::vec2(1, 0);

		data[2].Position = glm::vec3(x + width, y + height, 0.1f);
		data[2].TexCoord = glm::vec2(1, 1);

		data[3].Position = glm::vec3(x, y + height, 0.1f);
		data[3].TexCoord = glm::vec2(0, 1);

		PipelineSpecification pipelineSpecification;
		pipelineSpecification.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		s_Data->m_FullscreenQuadPipeline = Pipeline::Create(pipelineSpecification);
		Memory::Buffer buffer(data, 4 * sizeof(QuadVertex));
		s_Data->m_FullscreenQuadVertexBuffer = VertexBuffer::Create(buffer);
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		s_Data->m_FullscreenQuadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));
	}

	void Rendering::BindRenderingPass(const Memory::Shared<RenderingPass>& pass)
	{
		RADIANT_VERIFY(pass, "Render pass cannot be null!");
		s_Data->ActiveRenderingPass = pass;

		pass->GetSpecification().TargetFramebuffer->Bind();

	}

	void Rendering::UnbindRenderingPass()
	{
		RADIANT_VERIFY(s_Data->ActiveRenderingPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");

		s_Data->ActiveRenderingPass->GetSpecification().TargetFramebuffer->Unbind();
		s_Data->ActiveRenderingPass = nullptr;
	}

	void Rendering::SubmitCommand(std::function<void()> func)
	{
		s_Data->s_CommandBuffer->AddCommand(func);
	}

	void Rendering::ExecuteCommand()
	{
		s_Data->s_CommandBuffer->Execute();
	}

	Memory::CommandBuffer& GetRenderingCommandBuffer()
	{
		return *s_Data->s_CommandBuffer;
	}
}