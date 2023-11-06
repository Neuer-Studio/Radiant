#pragma once

namespace Radiant
{
	enum class ImageFormat
	{
		None = 0,
		RGB,
		SRGB8,
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