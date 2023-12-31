#pragma once

#include "VertexBuffer.hpp"

namespace Radiant
{

	struct PipelineSpecification
	{
		std::string DebugName = "DebugName";
		VertexBufferLayout Layout;
	};

	class Pipeline : public Memory::RefCounted
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual const PipelineSpecification& GetSpecification() const = 0;

		virtual void Invalidate() = 0;

		virtual void Bind() const= 0;
		virtual void Unbind() const= 0;

		static Memory::Shared<Pipeline> Create(const PipelineSpecification& spec);
	};

}
