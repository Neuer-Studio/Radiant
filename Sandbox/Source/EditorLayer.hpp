#pragma once

#include <Radiant/Memory/Buffer.hpp>

#include <Radiant/Rendering/VertexBuffer.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <Radiant/Rendering/Shader.hpp>
#include <Radiant/Rendering/Texture.hpp>
#include <Radiant/Rendering/Pipeline.hpp>

#include <Radiant/Rendering/Platform/OpenGL/OpenGLShader.hpp>

#include <glm/glm.hpp>
#include <imgui/imgui.h>

namespace Radiant
{
	class EditorLayer : public Layer 
	{
	public:
		EditorLayer()
			: Layer("EditorLayer")
		{}

		virtual void OnAttach()
		{
			Memory::Buffer b;
			float* v = new float[6]
			{
				0.0f, 0.5f,
					-0.5f, -0.5f,
					0.5f, -0.5f
			};
			b = Memory::Buffer::Copy(v, 6 * 4);
			VBO = VertexBuffer::Create(b);

			sh = Shader::Create("Resources/Shaders/TestShader.rads");
			tex = Texture2D::Create("Resources/Textures/BRDF_LUT.tga");
			VertexBufferLayout vertexLayout;
			vertexLayout = {
					{ ShaderDataType::Float3, "a_Position" },
			};
			PipelineSpecification pipelineSpecification;
			pipelineSpecification.Layout = vertexLayout;
			pip = Pipeline::Create(pipelineSpecification);
		}
		virtual void OnDetach() 
		{

		}
		virtual void OnUpdate() override
		{
			Rendering::Clear();

			sh.As<OpenGLShader>()->SetFloat3("u_Color", color);
			VBO->Bind();
			sh->Bind();
			pip->Bind();
			Rendering::Draw();
		}

		virtual void OnImGuiRender() override
		{
			ImGui::Begin("Color");
			ImGui::ColorEdit3("Color", &color[0]);
			ImGui::End();
		}
	private:
		Memory::Ref<VertexBuffer> VBO;
		Memory::Ref<Shader> sh;
		Memory::Ref<Texture2D> tex;
		Memory::Ref<Pipeline> pip;

		glm::vec3 color;
	};
}