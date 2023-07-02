#pragma once

#include <Radiant/Memory/Buffer.hpp>
#include <Radiant/Rendering/RenderingTypes.hpp>

namespace Radiant
{
	enum class VertexBufferUsage
	{
		None = 0, Static = 1, Dynamic = 2
	};

	class VertexBuffer : public Memory::RefCounted
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void SetData(Memory::Buffer buffer, std::size_t offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual std::size_t GetCount() const = 0;
		virtual std::size_t GetSize() const = 0;

		static Memory::Ref<VertexBuffer> Create(Memory::Buffer data, VertexBufferUsage usage = VertexBufferUsage::Static);
		static Memory::Ref<VertexBuffer> Create(std::size_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
	};
}