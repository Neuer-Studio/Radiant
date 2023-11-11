#include <Radiant/Rendering/Mesh.hpp>
#include <Radiant/Rendering/ShaderConstants.hpp>

#include <glad/glad.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <Radiant/Rendering/Rendering.hpp>

namespace Radiant
{
#define MESH_DEBUG_LOG 1
#if MESH_DEBUG_LOG
#define MESH_LOG(...) RA_INFO(__VA_ARGS__)
#else
#define MESH_LOG(...)
#endif


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
		using namespace Constants;
		LogStream::Initialize();

		RADIANT_VERIFY(Utils::FileSystem::Exists(filepath));
		RA_INFO("Loading static mesh: {0}", filepath.c_str());

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filepath, ImportFlags);
		if (!scene || !scene->HasMeshes())
			RADIANT_VERIFY("Failed to load mesh file: {0}", filepath);

		m_Shader = Rendering::GetShaderLibrary()->Get("Static_Shader.rads");

		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		m_Submeshes.reserve(scene->mNumMeshes);
		m_Materials.resize(scene->mNumMeshes);
		m_Textures.resize(scene->mNumMeshes);

		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			Submesh submesh;
			submesh.Vertex = vertexCount;
			submesh.Index = indexCount;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.IndexCount = mesh->mNumFaces * 3;
			submesh.Name = mesh->mName.C_Str();

			m_Submeshes.push_back(submesh);

			vertexCount += mesh->mNumVertices;
			indexCount += submesh.IndexCount;

			RADIANT_VERIFY(mesh->HasPositions(), "Meshes require positions.");
			RADIANT_VERIFY(mesh->HasNormals(), "Meshes require normals.");

			// Extract vertices from model
			for (size_t i = 0; i < mesh->mNumVertices; i++)
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

			// Extract indices from model
			for (size_t i = 0; i < mesh->mNumFaces; i++)
			{
				RADIANT_VERIFY(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
				m_Indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
			}
		}

		if (scene->HasMaterials())
		{
			MESH_LOG("=====================================", filepath);
			MESH_LOG("====== Materials - {0} ======", filepath);
			MESH_LOG("=====================================", filepath);

			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				auto aiMaterial = scene->mMaterials[i];
				auto aiMaterialName = aiMaterial->GetName();

				auto materialInstance = Material::Create(m_Shader, aiMaterialName.C_Str());

				aiString aiTexPath;
				glm::vec3 albedoColor(0.8f);
				aiColor3D aiColor;
				if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == aiReturn_SUCCESS)
					albedoColor = { aiColor.r, aiColor.g, aiColor.b };

				bool hasAlbedoTexture = aiMaterial->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
				if (hasAlbedoTexture)
				{
					std::filesystem::path imagePath = Utils::FileSystem::GetFileDirectory(filepath) / std::filesystem::path(aiTexPath.C_Str());
					MESH_LOG("		Albedo path = {}", imagePath.string());

					auto texture = Texture2D::Create(imagePath, true);
					if (texture->Loaded())
					{
						m_Textures[i] = texture;
						materialInstance->SetValue(ShaderUniforms::GetAlbedoTexture(), texture);
						materialInstance->SetValue(ShaderUniforms::GetAlbedoTextureToggle(), true, UniformTarget::Fragment);
					}

					else
					{
						materialInstance->SetValue(ShaderUniforms::GetAlbedoColor(), albedoColor, UniformTarget::Fragment);
						MESH_LOG("		No albedo texture");

					}

				}
				else
				{
					materialInstance->SetValue(ShaderUniforms::GetAlbedoTextureToggle(), false, UniformTarget::Fragment);
					materialInstance->SetValue(ShaderUniforms::GetAlbedoColor(), albedoColor, UniformTarget::Fragment);
				}

				// Normal texture
				bool hasNormalTexture = aiMaterial->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS;
				if (hasNormalTexture)
				{
					std::filesystem::path imagePath = Utils::FileSystem::GetFileDirectory(filepath) / std::filesystem::path(aiTexPath.C_Str());
					MESH_LOG("		Normal path = {}", imagePath.string());

					auto texture = Texture2D::Create(imagePath, true);
					if (texture->Loaded())
					{
						m_Textures[i] = texture;
						materialInstance->SetValue(ShaderUniforms::GetNormalTexture(), texture);
						materialInstance->SetValue(ShaderUniforms::GetAlbedoTextureToggle(), true, UniformTarget::Fragment);
					}

					else
					{
						MESH_LOG("		No Normal texture");

					}
				}

				// Shininess Texture
				bool hasShininessTexture = aiMaterial->GetTexture(aiTextureType::aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS;
				if (hasShininessTexture)
				{
					std::filesystem::path imagePath = Utils::FileSystem::GetFileDirectory(filepath) / std::filesystem::path(aiTexPath.C_Str());
					MESH_LOG("		Roughness path = {}", imagePath.string());

					auto texture = Texture2D::Create(imagePath, true);
					if (texture->Loaded())
					{
						m_Textures[i] = texture;
						materialInstance->SetValue(ShaderUniforms::GetRoughnessTexture(), texture);
					}

					else
					{
						MESH_LOG("		No Shininess texture");
						materialInstance->SetValue(ShaderUniforms::GetRoughness(), 1.0f, UniformTarget::Fragment);
						materialInstance->SetValue(ShaderUniforms::GetRoughnessTextureToggle(), true, UniformTarget::Fragment);
					}
				}

				if (aiMaterial->Get("$raw.ReflectionFactor|file", aiPTI_String, 0, aiTexPath) == AI_SUCCESS)
				{
					std::filesystem::path imagePath = Utils::FileSystem::GetFileDirectory(filepath) / std::filesystem::path(aiTexPath.C_Str());
					MESH_LOG("		Metalness path = {}", imagePath.string());

					auto texture = Texture2D::Create(imagePath, true);
					if (texture->Loaded())
					{
						m_Textures[i] = texture;
						materialInstance->SetValue(ShaderUniforms::GetMetalnessTexture(), texture);
						materialInstance->SetValue(ShaderUniforms::GetMetalnessTextureToggle(), true, UniformTarget::Fragment);
					}

					else
					{
						MESH_LOG("		No Metalness texture");
						materialInstance->SetValue(ShaderUniforms::GetMetalness(), 0.5f, UniformTarget::Fragment);
						materialInstance->SetValue(ShaderUniforms::GetMetalnessTextureToggle(), true, UniformTarget::Fragment);
					}
				}

				m_Materials[i] = materialInstance;
			}
		}

		Memory::Buffer buffer((std::byte*)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
		m_VertexBuffer = VertexBuffer::Create("Mesh", buffer);

		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));

	}
}