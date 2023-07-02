#pragma once

#include "Memory/Buffer.hpp"

namespace Radiant
{
	class IndexBuffer : public Memory::RefCounted
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void SetData(Memory::Buffer buffer, std::size_t offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual std::size_t GetCount() const = 0;
		virtual std::size_t GetSize() const = 0;

		static Memory::Ref<IndexBuffer> Create(Memory::Buffer data);
		static Memory::Ref<IndexBuffer> Create(std::size_t size);
	};
}
