#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <Radiant/Rendering/VertexBuffer.hpp>
#include <Radiant/Rendering/IndexBuffer.hpp>
#include <Radiant/Rendering/Pipeline.hpp>

namespace Assimp
{
	class Importer;
}

namespace Radiant
{
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
		Mesh(const std::string& filename);
		~Mesh() {}

		Memory::Shared<Pipeline> GetPipeline() { return m_Pipeline; }
		void Update();
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		Memory::Shared<Pipeline> m_Pipeline;
		Memory::Shared<VertexBuffer> m_VertexBuffer;
		Memory::Shared<IndexBuffer> m_IndexBuffer;
	};
}