#include <Radiant/Animation/AssimpAnimationImporter.hpp>
#include <Radiant/Animation/Animation.hpp>

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
		T Value;
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
		uint32_t Index; 
	};

	static std::vector<Channel> ImportChannels(const aiAnimation* anim, const Skeleton& skeleton)
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

		channels.resize(skeleton.GetNumBones());
		for (uint32_t i = 0; i < skeleton.GetNumBones(); i++)
		{
			channels[i].Index = i;

			auto validChannel = validChannels.find(i);
			if (validChannel != validChannels.end())
			{
				auto nodeAnim = validChannel->second;
				channels[i].Translations.reserve(nodeAnim->mNumPositionKeys);
				channels[i].Rotations.reserve(nodeAnim->mNumRotationKeys);
				channels[i].Scales.reserve(nodeAnim->mNumScalingKeys);

				for (uint32_t keyIndex = 0; keyIndex < nodeAnim->mNumPositionKeys; keyIndex++)
				{
					auto key = nodeAnim->mPositionKeys[keyIndex];
					float frameTime = std::clamp(static_cast<float>(key.mTime / anim->mDuration), 0.0f, 1.0f);

					if ((keyIndex == 0) && (frameTime > 0))
					{
						channels[i].Translations.push_back({ 0.0f, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z) });
					}
					channels[i].Translations.push_back({ 0.0f, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z) });
				}

				if (channels[i].Translations.empty())
				{
					RADIANT_VERIFY("No translation track found for bone '{}'", skeleton.GetBoneName(i));
					channels[i].Translations = { {0.0f, glm::vec3{0.0f}}, {1.0f, glm::vec3{0.0f}} };
				}
				else if (channels[i].Translations.back().FrameTime < 1.0f)
				{
					channels[i].Translations.emplace_back(1.0f, channels[i].Translations.back().Value);
				}

				for (uint32_t keyIndex = 0; keyIndex < nodeAnim->mNumRotationKeys; keyIndex++)
				{
					auto key = nodeAnim->mRotationKeys[keyIndex];
					float frameTime = std::clamp(static_cast<float>(key.mTime / anim->mDuration), 0.0f, 1.0f);

					if ((keyIndex == 0) && (frameTime > 0))
					{
						channels[i].Rotations.push_back({ 0.0f, glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z) });
					}
					channels[i].Rotations.push_back({ 0.0f, glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z) });
				}

				if (channels[i].Rotations.empty())
				{
					RADIANT_VERIFY("No translation track found for bone '{}'", skeleton.GetBoneName(i));
					channels[i].Rotations = { {0.0f, glm::vec3{0.0f}}, {1.0f, glm::vec3{0.0f}} };
				}
				else if (channels[i].Rotations.back().FrameTime < 1.0f)
				{
					channels[i].Rotations.emplace_back(1.0f, channels[i].Rotations.back().Value);
				}

				for (uint32_t keyIndex = 0; keyIndex < nodeAnim->mNumScalingKeys; keyIndex++)
				{
					auto key = nodeAnim->mScalingKeys[keyIndex];
					float frameTime = std::clamp(static_cast<float>(key.mTime / anim->mDuration), 0.0f, 1.0f);

					if ((keyIndex == 0) && (frameTime > 0))
					{
						channels[i].Scales.push_back({ 0.0f, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z) });
					}
					channels[i].Scales.push_back({ 0.0f, glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z) });
				}

				if (channels[i].Scales.empty())
				{
					RADIANT_VERIFY("No translation track found for bone '{}'", skeleton.GetBoneName(i));
					channels[i].Scales = { {0.0f, glm::vec3{0.0f}}, {1.0f, glm::vec3{0.0f}} };
				}
				else if (channels[i].Scales.back().FrameTime < 1.0f)
				{
					channels[i].Scales.emplace_back(1.0f, channels[i].Scales.back().Value);
				}
			}
		}

		return channels;
	}

	Animation ImportAnimation(const aiScene* scene, const std::string_view animationName, const Skeleton& skeleton)
	{
		for (uint32_t i = 0; i < scene->mNumAnimations; i++)
		{
			const aiAnimation* anim = scene->mAnimations[i];
			if (anim->mName.C_Str() == animationName)
			{
				auto channels = ImportChannels(anim, skeleton);

				std::vector<TranslationKey> translationKeys;
				std::vector<RotationKey> rotationKeys;
				std::vector<ScaleKey> scaleKeys;

				uint32_t numTranslations = 0;
				uint32_t numRotations = 0;
				uint32_t numScales = 0;

				for (auto channel : channels)
				{
					numTranslations += static_cast<uint32_t>(channel.Translations.size());
					numRotations += static_cast<uint32_t>(channel.Rotations.size());
					numScales += static_cast<uint32_t>(channel.Scales.size());
				}

				translationKeys.reserve(numTranslations);
				rotationKeys.reserve(numRotations);
				scaleKeys.reserve(numScales);

				for (const auto& channel : channels)
				{
					for (const auto& translation : channel.Translations)
					{
						translationKeys.push_back({ translation.FrameTime, channel.Index, translation.Value });
					}

					for (const auto& translation : channel.Rotations)
					{
						rotationKeys.push_back({ translation.FrameTime, channel.Index, translation.Value });
					}

					for (const auto& translation : channel.Scales)
					{
						scaleKeys.push_back({ translation.FrameTime, channel.Index, translation.Value });
					}
				}

				double samplingRate = anim->mTicksPerSecond;
				if (samplingRate < 0.0001)
				{
					samplingRate = 1.0;
				}

				Animation animation(animationName, static_cast<float>(anim->mDuration / samplingRate));
				animation.SetKeyFrames(translationKeys, rotationKeys, scaleKeys);

				return animation;
			}
		}
		return Animation("Null", -1.0f);
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