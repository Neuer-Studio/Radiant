#pragma once

#include <Radiant/Memory/Buffer.hpp>

#include <Radiant/Rendering/VertexBuffer.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <Radiant/Rendering/Shader.hpp>
#include <Radiant/Rendering/Texture.hpp>
#include <Radiant/Rendering/Pipeline.hpp>

#include <Radiant/Rendering/Platform/OpenGL/OpenGLShader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <Radiant/Core/Camera.hpp>

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
			float* v = new float[20]
			{
				 // positions          // texture coords
				 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
				 0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
				-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
				-0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
			};
			b = Memory::Buffer::Copy(v, 20 * 4);
			VBO = VertexBuffer::Create(b);

			sh = Shader::Create("Resources/Shaders/TestShader.rads");
			tex = Texture2D::Create("Resources/Textures/Tetxure.jpg");
			tex1 = Texture2D::Create("Resources/Textures/awesomeface.png");
			VertexBufferLayout vertexLayout;
			vertexLayout = {
					{ ShaderDataType::Float3, "a_Position" },
					{ ShaderDataType::Float2, "a_TexCoords" },
			};
			PipelineSpecification pipelineSpecification;
			pipelineSpecification.Layout = vertexLayout;
			pip = Pipeline::Create(pipelineSpecification);

			camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), (float)1600.f, (float)1600.f, 10.1f, 10000.0f));
		}
		virtual void OnDetach() 
		{

		}
		virtual void OnUpdate() override
		{
			camera.Update();
			auto viewProjection = camera.GetProjectionMatrix() * camera.GetViewMatrix();
			Rendering::Clear();
			sh->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
			tex->Bind(0);
			tex1->Bind(1);
			VBO->Bind();
			pip->Bind();
			sh->Bind();
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
		Memory::Ref<Texture2D> tex1;
		Memory::Ref<Pipeline> pip;

		Camera camera;

		glm::vec3 color;
	};
}