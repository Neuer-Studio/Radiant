#include <Radiant/Animation/Skeleton.hpp>

#include <Radiant/Math/Math.hpp>

namespace Radiant
{
	Skeleton::Skeleton(uint32_t size)
	{
		m_BoneNames.reserve(size);
	}

	void Skeleton::AddBone(const std::string& name, const glm::mat4& transform)
	{
		m_BoneNames.push_back(name);

		glm::vec3 translation, scale;
		glm::quat rotation;
		
		Math::DecomposeTransform(transform, translation, rotation, scale);

		m_BoneTranslations.push_back(translation);
		m_BoneRotations.push_back(rotation);
		m_BoneScales.push_back(scale);
	}

	uint32_t Skeleton::GetBoneIndex(const std::string_view name) const
	{
		for (size_t i = 0; i < m_BoneNames.size(); ++i)
		{
			if (m_BoneNames[i] == name)
			{
				return static_cast<uint32_t>(i);
			}
		}
		return -1;
	}

}