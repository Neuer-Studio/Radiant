#pragma once

#include <Radiant/Memory/Buffer.hpp>

#include <Rendering/VertexBuffer.hpp>
#include <Rendering/Rendering.hpp>

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
	private:
		Memory::Ref<VertexBuffer> VBO;
	};
}