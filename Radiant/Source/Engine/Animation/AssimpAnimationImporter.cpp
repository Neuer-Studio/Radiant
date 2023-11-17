#include <Radiant/Animation/AssimpAnimationImporter.hpp>

namespace Radiant::AssimpAnimationImporter
{
	namespace Utils
	{
		glm::mat4 Mat4FromAIMatrix4x4(const aiMatrix4x4& matrix)
		{
			glm::mat4 result;
			//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
			result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
			result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
			result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
			result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
			return result;
		}

	}
	class BoneList
	{
	public:
		BoneList(const aiScene* scene)
			: m_Scene(scene)
		{

		}

		Skeleton CreaetSkeleton()
		{
			if (!m_Scene)
			{
				return {};
			}

			ExtractBones();
			if (m_Bones.empty())
			{
				return {};
			}

			auto skeleton = Skeleton(static_cast<uint32_t>(m_Bones.size()));
			TraverseNode(m_Scene->mRootNode, &skeleton);

			return skeleton;
		}
	private:
		void ExtractBones()
		{
			for (uint32_t mi = 0; mi < m_Scene->mNumMeshes; mi++)
			{
				auto mesh = m_Scene->mMeshes[mi];

				for (uint32_t bi = 0; bi < mesh->mNumBones; bi++)
				{
					m_Bones.emplace(mesh->mBones[bi]->mName.C_Str());
				}
			}
		}

		void TraverseBone(const aiNode* node, Skeleton* skeleton)
		{
			skeleton->AddBone(node->mName.C_Str(), Utils::Mat4FromAIMatrix4x4(node->mTransformation));
			for (uint32_t nodeIndex = 0; nodeIndex < node->mNumChildren; ++nodeIndex)
			{
				TraverseBone(node->mChildren[nodeIndex], skeleton);
			}
		}

		void TraverseNode(const aiNode* node, Skeleton* skeleton)
		{
			if (m_Bones.find(node->mName.C_Str()) != m_Bones.end())
			{
				TraverseBone(node, skeleton);
			}
			else
			{
				for (uint32_t nodeIndex = 0; nodeIndex < node->mNumChildren; ++nodeIndex)
				{
					TraverseNode(node->mChildren[nodeIndex], skeleton);
				}
			}

		}
	private:
		const aiScene* m_Scene;
		std::set<std::string_view> m_Bones;
	};

	Skeleton ImportSkeleton(const aiScene* scene)
	{
		BoneList list(scene);
		return list.CreaetSkeleton();
	}

	template<typename T>
	struct KeyFrame
	{
		T value;
		float FrameTime;

		KeyFrame(const float frameTime, const T& value)
			: FrameTime(frameTime), Value(value)
		{

		}
	};

	struct Channel
	{
		std::vector<KeyFrame<glm::vec3>> Translations;
		std::vector<KeyFrame<glm::quat>> Rotations;
		std::vector<KeyFrame<glm::vec3>> Scales;
	};

	std::vector<Channel> ImportChannels(const aiAnimation* anim, const Skeleton& skeleton)
	{
		std::vector<Channel> channels;
		std::unordered_map<std::string_view, uint32_t> boneIndices;

		for (uint32_t i = 0; i < skeleton.GetNumBones(); i++)
		{
			boneIndices[skeleton.GetBoneName(i)] = i;
		}

		std::map<uint32_t, aiNodeAnim*> validChannels;
		for (uint32_t i = 0; i < anim->mNumChannels; i++)
		{
			aiNodeAnim* nodeAnim = anim->mChannels[i];
			auto it = boneIndices.find(nodeAnim->mNodeName.C_Str());
			if (it != boneIndices.end())
				validChannels[it->second] = nodeAnim;
		}

		return {};
	}

	Animation ImportAnimation(const aiScene* scene, const std::string_view animationName, const Skeleton& skeleton)
	{
		for (uint32_t i = 0; i < scene->mNumAnimations; i++)
		{
			const aiAnimation* anim = scene->mAnimations[i];
			if (anim->mName.C_Str() == animationName)
			{
				auto channels = ImportChannels(anim, skeleton);
			}
		}
		return Animation(animationName, 2.0f);
	}

	std::vector<std::string> GetAnimationNames(const aiScene* scene)
	{
		std::vector<std::string> names;
		names.reserve(scene->mNumAnimations);
		for (uint32_t i = 0; i < scene->mNumAnimations; i++)
		{
			names.push_back(scene->mAnimations[i]->mName.C_Str());
		}

		return names;
	}
}