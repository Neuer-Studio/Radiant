#pragma once

#include <Radiant/Memory/Buffer.hpp>

#include <Radiant/Rendering/VertexBuffer.hpp>
#include <Radiant/Rendering/Rendering.hpp>

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

		}
		virtual void OnDetach() 
		{

		}
		virtual void OnUpdate() override
		{
			Rendering::Clear();

			VBO->Bind();
			Rendering::Draw();
		}

		virtual void OnImGuiRender() override
		{

		}
	private:
		Memory::Ref<VertexBuffer> VBO;
	};
}