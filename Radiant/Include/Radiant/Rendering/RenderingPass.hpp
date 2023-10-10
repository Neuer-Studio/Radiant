#pragma once

#include <Radiant/Rendering/Framebuffer.hpp>

namespace Radiant
{
	struct RenderingPassSpecification
	{
		Memory::Shared<Framebuffer> Framebuffer;
	};

	class RenderingPass : public Memory::RefCounted
	{
	public:
		virtual ~RenderingPass() = default;

		virtual RenderingPassSpecification& GetSpecification() = 0;
		virtual const RenderingPassSpecification& GetSpecification() const = 0;

		static Memory::Shared<RenderingPass> Create(const RenderingPassSpecification& spec);
	};
}
