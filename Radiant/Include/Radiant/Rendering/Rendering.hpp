#pragma once

#include <Radiant/Memory/CommandBuffer.hpp>
#include <Radiant/Rendering/RenderingPass.hpp>
#include <Radiant/Rendering/Mesh.hpp>

namespace Radiant
{
	class TextureCube;
	class OpenGLRenderingPass;
	class SceneRenderingPanel;

	class Rendering 
	{
	public:
		static void Init();
		static void Shutdown();

		static void DrawMesh(Memory::Shared<Mesh> mesh);
		static void DrawMeshWithShader();

		static void DrawFullscreenQuad();

		static void BindRenderingPass(const Memory::Shared<RenderingPass>& pass);
		static void UnbindRenderingPass();

		static void SubmitCommand(std::function<void()> func);
		static void ExecuteCommand();

		static const Memory::Shared<ShaderLibrary>& GetShaderLibrary();
	private:
		static void DrawIndexed(std::size_t count, PrimitiveType type = PrimitiveType::Triangles, bool depthTest = false);
		static void Clear(float r, float g, float b);
	private:
	private:
		static Memory::CommandBuffer& GetRenderingCommandBuffer();

		friend Mesh;
		friend OpenGLRenderingPass;
		friend SceneRenderingPanel;
	};
}