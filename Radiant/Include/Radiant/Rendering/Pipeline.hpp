#pragma once

namespace Radiant
{

	struct PipelineSpecification
	{
		std::string DebugName;
	};

	class Pipeline : public Memory::RefCounted
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual const PipelineSpecification& GetSpecification() const = 0;

		virtual void Invalidate() = 0;

		virtual void Bind() = 0;

		static Memory::Ref<Pipeline> Create(const PipelineSpecification& spec);
	};

}
