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
#include <Radiant/Rendering/IndexBuffer.hpp>

namespace Radiant
{
	struct Material
	{
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};

	struct Light
	{
		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
			: Layer("EditorLayer")
		{}

		virtual void OnAttach()
		{
			Memory::Buffer buf;
			float v[]
			{
				// Front face
				-0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
				 0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
				 0.5f,  0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
				-0.5f,  0.5f, 0.5f,  0.0f, 0.0f, 1.0f,

				// Back face (Note the clockwise order)
				-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
				 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
				 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
				-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			};
			buf = Memory::Buffer::Copy(v, sizeof(v));

		
			m_CubeVertex = VertexBuffer::Create(buf);

			static uint32_t indices[] = {
				0, 1, 2, 2, 3, 0, // Передняя грань
				1, 5, 6, 6, 2, 1, // Правая грань
				7, 6, 5, 5, 4, 7, // Задняя грань
				4, 0, 3, 3, 7, 4, // Левая грань
				4, 5, 1, 1, 0, 4, // Нижняя грань
				3, 2, 6, 6, 7, 3  // Верхняя грань
			};

			m_CubeIndex = IndexBuffer::Create(indices, sizeof(indices));

			VertexBufferLayout vertexLayout;
			vertexLayout = {
					{ ShaderDataType::Float3, "a_Position" },
					{ ShaderDataType::Float3, "a_Normals" },
			};
			PipelineSpecification pipelineSpecification;
			pipelineSpecification.Layout = vertexLayout;

			m_Pipline = Pipeline::Create(pipelineSpecification);

			m_CubeShader = Shader::Create("Resources/Shaders/Colors.rads");

			m_Camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), (float)1600.f, (float)1600.f, 10.1f, 10000.0f));

			m_Material.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
			m_Material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
			m_Material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
			m_Material.shininess = 32.0f;

			m_Light.position = glm::vec3(1.0f, 2.0f, 3.0f);
			m_Light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
			m_Light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			m_Light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
		}

		virtual void OnDetach()
		{

		}

		virtual void OnUpdate() override
		{
			m_Camera.Update();
			Rendering::Clear();

			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_CubePosition);
			glm::mat4 viewMatrix = m_Camera.GetViewMatrix();
			glm::mat4 projectionMatrix = m_Camera.GetProjectionMatrix();

			m_Pipline->Bind(); /* Bind first */
			m_CubeVertex->Bind();
			m_CubeIndex->Bind();

			m_CubeShader->Bind();

			m_CubeShader->SetFloat3("u_Material.ambient", m_Material.ambient, UniformScope::Struct);
			m_CubeShader->SetFloat3("u_Material.diffuse", m_Material.diffuse, UniformScope::Struct);
			m_CubeShader->SetFloat3("u_Material.specular", m_Material.specular, UniformScope::Struct);
			m_CubeShader->SetFloat("u_Material.shininess", m_Material.shininess, UniformScope::Struct);

			m_CubeShader->SetFloat3("u_Light.position", m_Light.position, UniformScope::Struct);
			m_CubeShader->SetFloat3("u_Light.ambient", m_Light.ambient, UniformScope::Struct);
			m_CubeShader->SetFloat3("u_Light.diffuse", m_Light.diffuse, UniformScope::Struct);
			m_CubeShader->SetFloat3("u_Light.specular", m_Light.specular, UniformScope::Struct);

			m_CubeShader->SetMat4("u_Model", modelMatrix);
			m_CubeShader->SetMat4("u_View", viewMatrix);
			m_CubeShader->SetMat4("u_Projection", projectionMatrix);
			Rendering::DrawIndexed(m_CubeIndex->GetCount());

		}

		virtual void OnImGuiRender() override
		{
			ImGui::Begin("Cube");

			ImGui::DragFloat3("Cube Position", &m_CubePosition.x);
			ImGui::DragFloat3("Lights Dir", &m_Light.position.x);
			//ImGui::DragFloat3("Cube Position", &m_CubePosition.x);

			ImGui::End();
		}
	private:
		Memory::Ref<VertexBuffer> m_CubeVertex;
		Memory::Ref<IndexBuffer> m_CubeIndex;
		Memory::Ref<Shader> m_CubeShader;
		Memory::Ref<Pipeline> m_Pipline;

		Material m_Material; // Информация о материале
		Light m_Light;       // Информация об источнике света

		glm::vec3 m_CubePosition { 1.f };

		Camera m_Camera;
	};
}