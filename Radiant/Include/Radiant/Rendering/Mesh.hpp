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

	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normals;
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

		inline const std::string& GetFilePath() const { return m_FilePath; }
		inline const std::string& GetName() const { return m_Name; }
	private:
		std::string m_FilePath;
		std::string m_Name;

		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		Memory::Shared<VertexBuffer> m_VertexBuffer;
		Memory::Shared<Shader> m_Shader;
		Memory::Shared<IndexBuffer> m_IndexBuffer;
		Memory::Shared<Material> m_Material;

		friend Rendering;
		friend SceneRendering;
	};
}