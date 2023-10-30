#include <Radiant/Rendering/Mesh.hpp>

#include <glad/glad.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <Radiant/Rendering/Rendering.hpp>

namespace Radiant
{
	namespace {
		const unsigned int ImportFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_PreTransformVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_OptimizeMeshes |
			aiProcess_Debone |
			aiProcess_ValidateDataStructure;
	}

	struct LogStream : public Assimp::LogStream
	{
		static void Initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
			}
		}

		void write(const char* message) override
		{
			RA_ERROR("Assimp error: {0}", message);
		}
	};

	Mesh::Mesh(const std::string& filepath)
		: m_FilePath(filepath), m_Name(Utils::FileSystem::GetFileName(filepath))
	{
		LogStream::Initialize();

		RADIANT_VERIFY(Utils::FileSystem::Exists(filepath));
		RA_INFO("Loading static mesh: {0}", filepath.c_str());

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filepath, ImportFlags);
		if (!scene || !scene->HasMeshes())
			RADIANT_VERIFY("Failed to load mesh file: {0}", filepath);

		aiMesh* mesh = scene->mMeshes[0];

		RADIANT_VERIFY(mesh->HasPositions(), "Meshes require positions.");
		RADIANT_VERIFY(mesh->HasNormals(), "Meshes require normals.");

		m_Shader = Rendering::GetShaderLibrary()->Get("Static_Shader.rads");

		m_Vertices.reserve(mesh->mNumVertices);

		// Extract vertices from model
		for (size_t i = 0; i < m_Vertices.capacity(); i++)
		{
			Vertex vertex;
			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.Normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}

			if (mesh->HasTextureCoords(0))
				vertex.TexCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

			m_Vertices.push_back(vertex);
		}

		Memory::Buffer buffer((std::byte*)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
		m_VertexBuffer = VertexBuffer::Create("Mesh", buffer);

		// Extract indices from model
		m_Indices.reserve(mesh->mNumFaces);
		for (size_t i = 0; i < m_Indices.capacity(); i++)
		{
			RADIANT_VERIFY(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
			m_Indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
		}

		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));

		m_Material = Material::Create(m_Shader);
	}
}