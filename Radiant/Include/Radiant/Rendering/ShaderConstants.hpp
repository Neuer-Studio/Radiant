#pragma once

namespace Radiant::Constants
{
	class ShaderUniforms
	{
	public:
		// Albedo
		static const std::string GetAlbedoTexture() { return "u_AlbedoTexture"; }
		static const std::string GetAlbedoTextureToggle() { return "u_AlbedoTexToggle"; }
		static const std::string GetAlbedoColor() { return "u_MaterialUniform.AlbedoColor"; }

		// Normals

		static const std::string GetNormalTexture() { return "u_NormalTexture"; }
		static const std::string GetNormalTextureToggle() { return "u_NormalTexToggle"; }

		// Roughness

		static const std::string GetRoughnessTexture() { return "u_RoughnessTexture"; }
		static const std::string GetRoughnessTextureToggle() { return "u_RoughnessTexToggle"; }
		static const std::string GetRoughness() { return "u_MaterialUniform.Roughness"; }

		// Metalness

		static const std::string GetMetalnessTexture() { return "u_MetalnessTexture"; }
		static const std::string GetMetalnessTextureToggle() { return "u_MetalnessTexToggle"; }
		static const std::string GetMetalness() { return "u_MaterialUniform.AlbedoColor"; }
	};
}
