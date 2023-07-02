#pragma once

#include "Rendering/VertexBuffer.hpp"

namespace Radiant
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(Memory::Buffer buffer, VertexBufferUsage usage);
		OpenGLVertexBuffer(std::size_t size, VertexBufferUsage usage);
		~OpenGLVertexBuffer() override;

		virtual void SetData(Memory::Buffer buffer, std::size_t offset = 0) override;
		virtual void Bind() const override;

		virtual std::size_t GetCount() const override;
		virtual std::size_t GetSize() const override;
	private:
		Memory::Buffer m_Buffer;
		VertexBufferUsage m_Usage;
		std::size_t m_Size;

		RendererID m_RenderingID;
	};
}