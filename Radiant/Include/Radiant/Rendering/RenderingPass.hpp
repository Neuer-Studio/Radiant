#pragma once

#include <Radiant/Rendering/Framebuffer.hpp>

namespace Radiant
{
	struct RenderingPassSpecification
	{
		Memory::Shared<Framebuffer> TargetFramebuffer;
		bool clear = true;
	};

	class RenderingPass : public Memory::RefCounted
	{
	public:
		virtual ~RenderingPass() = default;

		virtual RenderingPassSpecification& GetSpecification() = 0;
		virtual const RenderingPassSpecification& GetSpecification() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Memory::Shared<RenderingPass> Create(const RenderingPassSpecification& spec);
	};
}
