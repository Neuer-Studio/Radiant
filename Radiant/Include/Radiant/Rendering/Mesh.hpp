#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <Radiant/Rendering/VertexBuffer.hpp>
#include <Radiant/Rendering/IndexBuffer.hpp>
#include <Radiant/Rendering/Pipeline.hpp>
#include <Radiant/Rendering/Shader.hpp>
#include <Radiant/Core/Timestep.hpp>
#include <Radiant/Rendering/Material.hpp>

#include <Radiant/Animation/Animation.hpp>
#include <Radiant/Animation/Skeleton.hpp>

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Assimp
{
	class Importer;
}

namespace Radiant
{
	class Rendering;
	class SceneRendering;
	class Material;

	const uint32_t BONEINFLUENCES = 4;

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

	struct BoneInfluence
	{
		Vertex vertex;

		uint32_t BoneInfoIndices[BONEINFLUENCES] = { 0 };
		float BoneWeight[BONEINFLUENCES] = { 0.f };

		void AddBoneData(uint32_t bone, float weight)
		{
			for(uint32_t i = 0; i < BONEINFLUENCES; i++)
			{
				if (!BoneWeight[i])
				{
					BoneInfoIndices[i] = bone;
					BoneWeight[i] = weight;
					return;
				}
			}

			RADIANT_VERIFY(false, "Too bany bones!");
		}
	};

	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
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

		bool HasSkeleton() const { return (bool)m_Skeleton.GetNumBones() > 0; }

		void UpdateAnimations(Timestep ts); // TODO: Create AnimationController component
	private:
		void BoneTransform(float time);
		void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

		const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);
		uint32_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		glm::vec3 InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
		glm::vec3 InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim);

	private:
		std::string m_FilePath;
		std::string m_Name;

		std::vector<Vertex> m_Vertices;
		std::vector<BoneInfluence> m_BoneInfluences;
		std::vector<BoneInfo> m_BoneInfo;
		std::vector<Submesh> m_Submeshes;
		std::vector<Index> m_Indices;

		uint32_t m_BoneCount = 0;
		std::unordered_map<std::string, uint32_t> m_BoneMapping;
		glm::mat4 m_InverseTransform;
		std::vector<glm::mat4> m_BoneTransforms;

		const aiScene* m_Scene;

		Skeleton m_Skeleton;
		std::vector<Animation> m_Animations;

		Memory::Shared<VertexBuffer> m_VertexBuffer;
		Memory::Shared<Shader> m_Shader;
		Memory::Shared<IndexBuffer> m_IndexBuffer;
		std::vector<Memory::Shared<Material>> m_Materials;
		std::vector<Memory::Shared<Texture2D>> m_Textures;

		friend Rendering;
		friend SceneRendering;
	};
}