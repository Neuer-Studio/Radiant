#include <Rendering/Rendering.hpp>
#include <Rendering/Platform/OpenGL/OpenGLRendering.hpp>

#include <Rendering/VertexBuffer.hpp>
#include <Rendering/IndexBuffer.hpp>
#include <Rendering/Pipeline.hpp>
#include <Rendering/Shader.hpp>

namespace Radiant
{
	struct QuadData
	{
		Memory::Shared<VertexBuffer> FullscreenQuadVertexBuffer;
		Memory::Shared<IndexBuffer> FullscreenQuadIndexBuffer;
		Memory::Shared<Pipeline> FullscreenQuadPipeline;
		Memory::Shared<Shader> FullscreenQuadShader;
	};

	struct RenderingData
	{
		Memory::CommandBuffer* CommandBuffer;
		RenderingAPI* RendererAPI;

		Memory::Shared<RenderingPass> ActiveRenderingPass;

		QuadData QuadInfo;
	};

	static RenderingData* s_Data = nullptr;

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
		s_Data = new RenderingData();

		RA_INFO("[Rendering] Init Rendering");
		s_Data->RendererAPI = InitRenderingAPI();
		s_Data->CommandBuffer = new Memory::CommandBuffer();

		s_Data->RendererAPI->Init();

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
		s_Data->QuadInfo.FullscreenQuadPipeline = Pipeline::Create(pipelineSpecification);
		Memory::Buffer buffer(data, 4 * sizeof(QuadVertex));
		s_Data->QuadInfo.FullscreenQuadVertexBuffer = VertexBuffer::Create(buffer);
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		s_Data->QuadInfo.FullscreenQuadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));
	}

	void Rendering::BindRenderingPass(const Memory::Shared<RenderingPass>& pass)
	{
		RADIANT_VERIFY(pass, "Render pass cannot be null!");
		s_Data->ActiveRenderingPass = pass;

		const auto passSpec = pass->GetSpecification();
		const auto fbspec = pass->GetSpecification().TargetFramebuffer->GetSpecification();

		passSpec.TargetFramebuffer->Bind();

		if (passSpec.clear)
			Clear(fbspec.ClearColor.r, fbspec.ClearColor.g, fbspec.ClearColor.b);

	}

	void Rendering::UnbindRenderingPass()
	{
		RADIANT_VERIFY(s_Data->ActiveRenderingPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");

		s_Data->ActiveRenderingPass->GetSpecification().TargetFramebuffer->Unbind();
		s_Data->ActiveRenderingPass = nullptr;
	}

	void Rendering::DrawQuad()
	{
		s_Data->QuadInfo.FullscreenQuadPipeline->Bind();
		s_Data->QuadInfo.FullscreenQuadIndexBuffer->Bind();
		s_Data->QuadInfo.FullscreenQuadVertexBuffer->Bind();
		s_Data->QuadInfo.FullscreenQuadShader->Bind();

		s_Data->QuadInfo.FullscreenQuadShader->SetValue("u_InverseVP", (std::byte*)nullptr, UniformTarget::Vertex);

		DrawIndexed(6);
	}

	void Rendering::SubmitCommand(std::function<void()> func)
	{
		s_Data->CommandBuffer->AddCommand(func);
	}

	void Rendering::ExecuteCommand()
	{
		s_Data->CommandBuffer->Execute();
	}

	Memory::CommandBuffer& GetRenderingCommandBuffer()
	{
		return *s_Data->CommandBuffer;
	}

	void Rendering::DrawIndexed(std::size_t count, PrimitiveType type, bool depthTest)
	{
		Rendering::SubmitCommand([=]()
			{
				s_Data->RendererAPI->DrawIndexed(count, type, depthTest);
			});
	}

	void Rendering::Clear(float r, float g, float b)
	{
		Rendering::SubmitCommand([=]()
			{
				s_Data->RendererAPI->Clear(r, g, b);
			});
	}
}