#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <Radiant/Rendering/VertexBuffer.hpp>
#include <Radiant/Rendering/IndexBuffer.hpp>
#include <Radiant/Rendering/Pipeline.hpp>
#include <Radiant/Rendering/Shader.hpp>
#include <Radiant/Rendering/Material.hpp>

namespace Assimp
{
	class Importer;
}

namespace Radiant
{
	class Rendering;
	class SceneRendering;
	class Material;

	struct Submesh
	{
		uint32_t Vertex;
		uint32_t Index;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
		std::string Name;

		glm::mat4 Transform;
	};

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normals;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 TexCoords;
	};

	struct Index
	{
		uint32_t V1, V2, V3;
	};

	class Mesh : public Memory::RefCounted
	{
	public:
		Mesh(const std::string& filepath);
		~Mesh() {}

		const std::string& GetFilePath() const { return m_FilePath; }
		const std::string& GetName() const { return m_Name; }
		Memory::Shared<Material>& GetMaterial() { return m_Materials[0]; }
		const Memory::Shared<Shader>& GetShader() { return m_Shader; }
		void SetShader(const Memory::Shared<Shader>& shader) { m_Shader = shader; }
		Memory::Shared<Material> GetAlbedoMaterial() const { return m_Materials[0]; }
	private:
		std::string m_FilePath;
		std::string m_Name;

		std::vector<Vertex> m_Vertices;
		std::vector<Submesh> m_Submeshes;
		std::vector<Index> m_Indices;

		Memory::Shared<VertexBuffer> m_VertexBuffer;
		Memory::Shared<Shader> m_Shader;
		Memory::Shared<IndexBuffer> m_IndexBuffer;
		std::vector<Memory::Shared<Material>> m_Materials;
		std::vector<Memory::Shared<Texture2D>> m_Textures;

		friend Rendering;
		friend SceneRendering;
	};
}