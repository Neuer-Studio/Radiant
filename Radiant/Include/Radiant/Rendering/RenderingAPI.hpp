#pragma once

#include "RenderingTypes.hpp"

namespace Radiant
{
	class Rendering;

	enum class RenderingAPIType : uint32_t
	{
		None = 0,
		Vulkan = 1,
		OpenGL = 2
	};

	struct GraphicsInfo {
		std::string Vendor;
		std::string Renderer;
		std::string Version;

		int MaxSamples = 0;
		float MaxAnisotropy = 0.0f;
		int MaxTextureUnits = 0;
	};

	class RenderingAPI : public Memory::RefCounted
	{
	public:
		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		static GraphicsInfo& GetGraphicsInfo()
		{
			static GraphicsInfo info;
			return info;
		}

		static RenderingAPIType GetAPI() { return s_RendererAPI; }
	private:
		virtual void DrawIndexed(uint32_t count, PrimitiveType type = PrimitiveType::Triangles, bool depthTest = false) = 0;
		virtual void Clear(float r, float g, float b) = 0;
	private:
		inline static RenderingAPIType s_RendererAPI = RenderingAPIType::OpenGL;

		friend Rendering;
	};
}