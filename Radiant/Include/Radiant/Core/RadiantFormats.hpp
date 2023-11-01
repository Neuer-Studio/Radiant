#pragma once

namespace Radiant
{
	enum class ImageFormat
	{
		None = 0,
		RGBA8,
		RGBA,
		RGBA16F,
		RGBA32F,

		// Depth
		DEPTH32F, 

		// Depth and stencil
		DEPTH24STENCIL8
	};
}