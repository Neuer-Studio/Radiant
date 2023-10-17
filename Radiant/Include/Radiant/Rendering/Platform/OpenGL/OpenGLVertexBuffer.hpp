#pragma once

#include "Rendering/VertexBuffer.hpp"

namespace Radiant
{
	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const std::string& tag, Memory::Buffer buffer, VertexBufferUsage usage);
		OpenGLVertexBuffer(const std::string& tag, std::size_t size, VertexBufferUsage usage);
		~OpenGLVertexBuffer() override;

		virtual void SetData(Memory::Buffer buffer, std::size_t offset = 0) override;
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual std::size_t GetCount() const override;
		virtual std::size_t GetSize() const override;
	private:
		Memory::Buffer m_Buffer;
		VertexBufferUsage m_Usage;
		std::size_t m_Size;
		std::string m_Tag;

		RendererID m_RenderingID = -1;
	};
}