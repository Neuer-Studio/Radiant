#include <Radiant/Animation/Animation.hpp>

namespace Radiant
{
	Animation::Animation(const std::string_view name, const float duration)
		: m_Name(name), m_Duration(duration)
	{}

	void Animation::SetKeyFrames(std::vector<TranslationKey> translations, std::vector<RotationKey> rotations, std::vector<ScaleKey> scales)
	{
		m_TranslationKeys = translations;
		m_RotationKeys = rotations;
		m_ScaleKeys = scales;
	}
}