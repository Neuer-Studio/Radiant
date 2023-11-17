#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Radiant
{
	class Skeleton
	{
	public:
		Skeleton() = default;
		~Skeleton() = default;
		Skeleton(uint32_t size);

		void AddBone(const std::string& name, const glm::mat4& transform);

		uint32_t GetNumBones() const { return static_cast<uint32_t>(m_BoneNames.size()); }
		const std::string& GetBoneName(uint32_t index) const { return m_BoneNames[index]; }

		const std::vector<glm::vec3>& GetBoneTranslations() const { return m_BoneTranslations; }
		const std::vector<glm::quat>& GetBoneRotations() const { return m_BoneRotations; }
		const std::vector<glm::vec3>& GetBoneScales() const { return m_BoneScales; }
	private:
		std::vector<std::string> m_BoneNames;

		std::vector<glm::vec3> m_BoneTranslations;
		std::vector<glm::quat> m_BoneRotations;
		std::vector<glm::vec3> m_BoneScales;
	};
}