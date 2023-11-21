#include <Radiant/Rendering/Mesh.hpp>
#include <Radiant/Rendering/ShaderConstants.hpp>

#include <glad/glad.h>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <Radiant/Rendering/Rendering.hpp>
#include <Radiant/Animation/AssimpAnimationImporter.hpp>
#include <Radiant/Math/Math.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Rendering/Platform/OpenGL/OpenGLShader.hpp>

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
			aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
			aiProcess_Triangulate |             // Make sure we're triangles
			aiProcess_SortByPType |             // Split meshes by primitive type
			aiProcess_GenNormals |              // Make sure we have legit normals
			aiProcess_GenUVCoords |             // Convert UVs if required 
			//		aiProcess_OptimizeGraph |
			aiProcess_OptimizeMeshes |          // Batch draws where possible
			aiProcess_JoinIdenticalVertices |
			aiProcess_LimitBoneWeights |        // If more than N (=4) bone weights, discard least influencing bones and renormalise sum to 1
			aiProcess_ValidateDataStructure |   // Validation
			aiProcess_GlobalScale;              // e.g. convert cm to m for fbx import (and other formats where cm is native)

	}
	static Assimp::Importer s_Importer;

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

		const aiScene* scene = s_Importer.ReadFile(filepath, ImportFlags);
		if (!scene || !scene->HasMeshes())
			RADIANT_VERIFY("Failed to load mesh file: {0}", filepath);
		
		m_Skeleton = AssimpAnimationImporter::ImportSkeleton(scene);
		if (HasSkeleton())
		{
			const auto animationNames = AssimpAnimationImporter::GetAnimationNames(scene);
			m_Animations.reserve(std::size(animationNames));
			for (const auto& animationName : animationNames)
			{
				m_Animations.emplace_back(AssimpAnimationImporter::ImportAnimation(scene, animationName, m_Skeleton));
			}
		}

		m_Shader = !HasSkeleton() ? Rendering::GetShaderLibrary()->Get("Static_Shader.rads") : Rendering::GetShaderLibrary()->Get("Animated_Shader.rads");
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		m_Submeshes.reserve(scene->mNumMeshes);
		m_Textures.resize(scene->mNumMeshes);

		for (int m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];
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

				BoneInfluence inf;
				inf.vertex = vertex;

				m_BoneInfluences.push_back(inf);

				m_Vertices.push_back(vertex);
			}

			// Extract indices from model
			for (size_t i = 0; i < mesh->mNumFaces; i++)
			{
				RADIANT_VERIFY(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
				m_Indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
			}
		}

		m_InverseTransform = glm::inverse(Math::Mat4FromAIMatrix4x4(scene->mRootNode->mTransformation));

		if (scene->HasMaterials())
		{
			MESH_LOG("=====================================", filepath);
			MESH_LOG("====== Materials - {0} ======", filepath);
			MESH_LOG("=====================================", filepath);

			m_Materials.resize(scene->mNumMaterials);

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

		//if (HasSkeleton())
		//{
		//	m_BoneInfluences.reserve(m_Vertices.size());

		//	for (uint32_t m = 0; m < scene->mNumMeshes; m++)
		//	{
		//		aiMesh* mesh = scene->mMeshes[m];
		//		Submesh& submesh = m_Submeshes[m];
		//		for (uint32_t i = 0; i < mesh->mNumBones; i++)
		//		{
		//			aiBone* bone = mesh->mBones[i];
		//			uint32_t boneIndex = m_Skeleton.GetBoneIndex(bone->mName.C_Str());

		//			if (boneIndex == -1)
		//			{
		//				RADIANT_VERIFY("Could not find mesh bone '{}' in skeleton!", bone->mName.C_Str());
		//			}

		//			uint32_t boneInfoIndex = ~0;
		//			for (size_t j = 0; j < m_BoneInfo.size(); ++j)
		//			{
		//				if ((m_BoneInfo[j].BoneIndex == boneIndex) && (m_BoneInfo[j].SubMeshIndex == m))
		//				{
		//					boneInfoIndex = static_cast<uint32_t>(j);
		//					break;
		//				}
		//			}
		//			if (boneInfoIndex == ~0)
		//			{
		//				boneInfoIndex = static_cast<uint32_t>(m_BoneInfo.size());
		//				m_BoneInfo.emplace_back(glm::inverse(submesh.Transform), Math::Mat4FromAIMatrix4x4(bone->mOffsetMatrix), m, boneIndex);
		//			}

		//			for (size_t j = 0; j < bone->mNumWeights; j++)
		//			{
		//				int VertexID = submesh.Vertex + bone->mWeights[j].mVertexId;
		//				float Weight = bone->mWeights[j].mWeight;
		//				m_BoneInfluences[VertexID].AddBoneData(boneInfoIndex, Weight);
		//			}

		//		}
		//	}
		//}

		for (size_t m = 0; m < scene->mNumMeshes; m++)
		{
			aiMesh* mesh = scene->mMeshes[m];
			Submesh& submesh = m_Submeshes[m];

			for (size_t i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* bone = mesh->mBones[i];
				std::string boneName(bone->mName.data);
				int boneIndex = 0;

				if (m_BoneMapping.find(boneName) == m_BoneMapping.end())
				{
					// Allocate an index for a new bone
					boneIndex = m_BoneCount;
					m_BoneCount++;
					BoneInfo bi;
					m_BoneInfo.push_back(bi);
					m_BoneInfo[boneIndex].BoneOffset = Math::Mat4FromAIMatrix4x4(bone->mOffsetMatrix);
					m_BoneMapping[boneName] = boneIndex;
				}
				else
				{
					RA_WARN("Found existing bone in map");
					boneIndex = m_BoneMapping[boneName];
				}

				for (size_t j = 0; j < bone->mNumWeights; j++)
				{
					int VertexID = submesh.Vertex + bone->mWeights[j].mVertexId;
					float Weight = bone->mWeights[j].mWeight;
					m_BoneInfluences[VertexID].AddBoneData(boneIndex, Weight);
				}
			}
		}



		if (HasSkeleton())
		{
			
			Memory::Buffer buffer((std::byte*)m_BoneInfluences.data(), m_BoneInfluences.size() * sizeof(BoneInfluence));
			m_VertexBuffer = VertexBuffer::Create("Mesh", buffer);
		}
		else
		{
			Memory::Buffer buffer((std::byte*)m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
			m_VertexBuffer = VertexBuffer::Create("Mesh", buffer);
		}

		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));
		m_Scene = scene;
	}

	uint32_t Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
		{
			if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}


	uint32_t Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		RADIANT_VERIFY(pNodeAnim->mNumRotationKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
		{
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}


	uint32_t Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		RADIANT_VERIFY(pNodeAnim->mNumScalingKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
		{
			if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}


	glm::vec3 Mesh::InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumPositionKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = nodeAnim->mPositionKeys[0].mValue;
			return { v.x, v.y, v.z };
		}

		uint32_t PositionIndex = FindPosition(animationTime, nodeAnim);
		uint32_t NextPositionIndex = (PositionIndex + 1);
		RADIANT_VERIFY(NextPositionIndex < nodeAnim->mNumPositionKeys);
		float DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (animationTime - (float)nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
		if (Factor < 0.0f)
			Factor = 0.0f;
		RADIANT_VERIFY(Factor <= 1.0f, "Factor must be below 1.0f");
		const aiVector3D& Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = nodeAnim->mPositionKeys[NextPositionIndex].mValue;
		aiVector3D Delta = End - Start;
		auto aiVec = Start + Factor * Delta;
		return { aiVec.x, aiVec.y, aiVec.z };
	}


	glm::quat Mesh::InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumRotationKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = nodeAnim->mRotationKeys[0].mValue;
			return glm::quat(v.w, v.x, v.y, v.z);
		}

		uint32_t RotationIndex = FindRotation(animationTime, nodeAnim);
		uint32_t NextRotationIndex = (RotationIndex + 1);
		RADIANT_VERIFY(NextRotationIndex < nodeAnim->mNumRotationKeys);
		float DeltaTime = (float)(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (animationTime - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
		if (Factor < 0.0f)
			Factor = 0.0f;
		RADIANT_VERIFY(Factor <= 1.0f, "Factor must be below 1.0f");
		const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
		auto q = aiQuaternion();
		aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
		q = q.Normalize();
		return glm::quat(q.w, q.x, q.y, q.z);
	}


	glm::vec3 Mesh::InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim)
	{
		if (nodeAnim->mNumScalingKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = nodeAnim->mScalingKeys[0].mValue;
			return { v.x, v.y, v.z };
		}

		uint32_t index = FindScaling(animationTime, nodeAnim);
		uint32_t nextIndex = (index + 1);
		RADIANT_VERIFY(nextIndex < nodeAnim->mNumScalingKeys);
		float deltaTime = (float)(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
		float factor = (animationTime - (float)nodeAnim->mScalingKeys[index].mTime) / deltaTime;
		if (factor < 0.0f)
			factor = 0.0f;
		RADIANT_VERIFY(factor <= 1.0f, "Factor must be below 1.0f");
		const auto& start = nodeAnim->mScalingKeys[index].mValue;
		const auto& end = nodeAnim->mScalingKeys[nextIndex].mValue;
		auto delta = end - start;
		auto aiVec = start + factor * delta;
		return { aiVec.x, aiVec.y, aiVec.z };
	}

	void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
	{
		std::string name(pNode->mName.data);
		const aiAnimation* animation = m_Scene->mAnimations[0];
		glm::mat4 nodeTransform(Math::Mat4FromAIMatrix4x4(pNode->mTransformation));
		const aiNodeAnim* nodeAnim = FindNodeAnim(animation, name);

		if (nodeAnim)
		{
			glm::vec3 translation = InterpolateTranslation(AnimationTime, nodeAnim);
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

			glm::quat rotation = InterpolateRotation(AnimationTime, nodeAnim);
			glm::mat4 rotationMatrix = glm::toMat4(rotation);

			glm::vec3 scale = InterpolateScale(AnimationTime, nodeAnim);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}

		glm::mat4 transform = ParentTransform * nodeTransform;

		if (m_BoneMapping.find(name) != m_BoneMapping.end())
		{
			uint32_t BoneIndex = m_BoneMapping[name];
			m_BoneInfo[BoneIndex].FinalTransformation = m_InverseTransform * transform * m_BoneInfo[BoneIndex].BoneOffset;
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], transform);
	}

	const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
	{
		for (uint32_t i = 0; i < animation->mNumChannels; i++)
		{
			const aiNodeAnim* nodeAnim = animation->mChannels[i];
			if (std::string(nodeAnim->mNodeName.data) == nodeName)
				return nodeAnim;
		}
		return nullptr;
	}

	void Mesh::BoneTransform(float time)
	{
		ReadNodeHierarchy(time, m_Scene->mRootNode, glm::mat4(1.0f));
		m_BoneTransforms.resize(m_BoneCount);
		for (size_t i = 0; i < m_BoneCount; i++)
			m_BoneTransforms[i] = m_BoneInfo[i].FinalTransformation;
	}


	void Mesh::UpdateAnimations(Timestep ts)
	{
		static auto m_AnimationTime = 0.0f;
		if (HasSkeleton())
		{

			float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f) * 1.0f;
			m_AnimationTime += 0.005 * ticksPerSecond;
			m_AnimationTime = fmod(m_AnimationTime, (float)m_Scene->mAnimations[0]->mDuration);
		}

		if (m_Scene->mAnimations)
			BoneTransform(m_AnimationTime);

		for (size_t i = 0; i < m_BoneTransforms.size(); i++)
		{
			std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
			auto pos = m_Shader.As<OpenGLShader>()->GetExternalUniformPosition(uniformName);
			m_Shader.As<OpenGLShader>()->UploadUniformMat4(pos, m_BoneTransforms[i], UniformTarget::Vertex);
		}

	}
}