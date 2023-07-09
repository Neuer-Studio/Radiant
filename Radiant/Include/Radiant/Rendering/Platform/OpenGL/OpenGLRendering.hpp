#pragma once

#include "Rendering/RenderingAPI.hpp"

namespace Radiant
{
	class OpenGLRendering final : public RenderingAPI
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;

		// NOTE: Tempory wile we don't have a rendering scene
		virtual void Draw() override;
		virtual void Clear() override;
	};
}