#pragma once

#include <Radiant/Memory/Buffer.hpp>

namespace Radiant
{
	class IndexBuffer : public Memory::RefCounted
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void SetData(void* data, uint32_t size, uint32_t offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual uint32_t GetSize() const = 0;
		virtual uint32_t GetCount() const = 0;

		static Memory::Ref<IndexBuffer> Create(void* data, uint32_t size);
		static Memory::Ref<IndexBuffer> Create(uint32_t size);
	};
}
