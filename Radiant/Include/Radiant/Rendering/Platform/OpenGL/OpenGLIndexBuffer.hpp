#pragma once

#include "Rendering/IndexBuffer.hpp"
#include <Rendering/RenderingTypes.hpp>

namespace Radiant
{
	class OpenGLIndexBuffer final : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(void* data, uint32_t size);
		OpenGLIndexBuffer(uint32_t size);

		~OpenGLIndexBuffer();

		virtual void SetData(void* data, uint32_t size, uint32_t offset) override;
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetSize() const { return m_Size; }
		virtual uint32_t GetCount() const { return m_Size / sizeof(uint32_t); }
	private:
		uint32_t m_Size = 0;
		RendererID m_RenderingID = -1;

		Memory::Buffer m_LocalData;
	};
}