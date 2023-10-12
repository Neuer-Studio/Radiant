#pragma once

#include "Rendering/RenderingAPI.hpp"

namespace Radiant
{
	class OpenGLRendering final : public RenderingAPI
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
	private:
		virtual void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest) override;
		virtual void Clear(float r, float g, float b) override;

		friend Rendering;
	};
}