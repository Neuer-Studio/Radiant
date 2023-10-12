#pragma once

#include <Radiant/Rendering/RenderingPass.hpp>

namespace Radiant
{
	class OpenGLRenderingPass final : public RenderingPass
	{
	public:
		OpenGLRenderingPass(const RenderingPassSpecification& spec);
		virtual ~OpenGLRenderingPass();

		virtual RenderingPassSpecification& GetSpecification() override { return m_Specification; }
		virtual const RenderingPassSpecification& GetSpecification() const override { return m_Specification; }
	private:
		RenderingPassSpecification m_Specification;
	};
}