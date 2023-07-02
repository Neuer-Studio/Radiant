#pragma once

#include "Rendering/IndexBuffer.hpp"
#include <Rendering/RenderingTypes.hpp>

namespace Radiant
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(Memory::Buffer data);
		OpenGLIndexBuffer(std::size_t size);
		virtual ~OpenGLIndexBuffer() override;

		virtual void SetData(Memory::Buffer buffer, std::size_t offset = 0) override;
		virtual void Bind() const override;

		virtual std::size_t GetCount() const override;
		virtual std::size_t GetSize() const override;
	private:
		Memory::Buffer m_Buffer;
		std::size_t m_Size;

		RendererID m_RenderingID;
	};
}