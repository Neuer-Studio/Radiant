#include <Rendering/Rendering.hpp>
#include <Rendering/Platform/OpenGL/OpenGLRendering.hpp>

#include <Rendering/VertexBuffer.hpp>
#include <Rendering/Texture.hpp>
#include <Rendering/IndexBuffer.hpp>
#include <Rendering/Pipeline.hpp>
#include <Rendering/Shader.hpp>


#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

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
		Memory::Shared<ShaderLibrary> RenderingShaders;

		Memory::Shared<RenderingPass> ActiveRenderingPass;
		Memory::Shared<Pipeline> PiplineStaticMesh;
		Memory::Shared<Pipeline> PiplineDynamicMesh;

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
		s_Data->QuadInfo.FullscreenQuadVertexBuffer = VertexBuffer::Create("Quad", buffer);
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		s_Data->QuadInfo.FullscreenQuadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));

		/***************** Static Mesh *****************/

		PipelineSpecification pipelineSpecificationStaticMesh;
		pipelineSpecificationStaticMesh.Layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normals" },
			{ ShaderDataType::Float3, "a_Tangent" },
			{ ShaderDataType::Float3, "a_Binormal" },
			{ ShaderDataType::Float2, "a_TexCoords" },
		};
		s_Data->PiplineStaticMesh = Pipeline::Create(pipelineSpecificationStaticMesh);

		{
			s_Data->RenderingShaders = Memory::Shared<ShaderLibrary>::Create();
			s_Data->RenderingShaders->Load("Resources/Shaders/Static_Shader.rads");
			s_Data->RenderingShaders->Load("Resources/Shaders/Quad.rads");
		}
	}

	const Memory::Shared<ShaderLibrary>& Rendering::GetShaderLibrary()
	{
		return s_Data->RenderingShaders;
	}

	void Rendering::BindRenderingPass(const Memory::Shared<RenderingPass>& pass)
	{
		RADIANT_VERIFY(pass, "Render pass cannot be null!");
		s_Data->ActiveRenderingPass = pass;

		pass->Bind();
	}

	void Rendering::UnbindRenderingPass()
	{
		RADIANT_VERIFY(s_Data->ActiveRenderingPass, "No active render pass! Have you called Rendernig::UnbindRenderingPass twice?");

		s_Data->ActiveRenderingPass->Unbind();
		s_Data->ActiveRenderingPass = nullptr;
	}

	void Rendering::DrawFullscreenQuad(Memory::Shared<Material> material)
	{
		s_Data->QuadInfo.FullscreenQuadVertexBuffer->Bind();
		s_Data->QuadInfo.FullscreenQuadPipeline->Bind();
		s_Data->QuadInfo.FullscreenQuadIndexBuffer->Bind();
		if(material.Ptr() != nullptr)
			material->UpdateForRendering();

		DrawIndexed(s_Data->QuadInfo.FullscreenQuadIndexBuffer->GetCount());
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

	void Rendering::DrawMesh(Memory::Shared<Mesh> mesh, Memory::Shared<Material> material)
	{
		mesh->m_VertexBuffer->Bind();
		s_Data->PiplineStaticMesh->Bind();
		mesh->m_IndexBuffer->Bind();
		material->UpdateForRendering();

		DrawIndexed(mesh->m_IndexBuffer->GetCount());
	}

	void Rendering::DrawMeshWithShader(Memory::Shared<Mesh> mesh, Memory::Shared<Shader> shader)
	{
		mesh->m_VertexBuffer->Bind();
		s_Data->PiplineStaticMesh->Bind();
		mesh->m_IndexBuffer->Bind();

		shader->Bind();

		DrawIndexed(mesh->m_IndexBuffer->GetCount());
	}
}