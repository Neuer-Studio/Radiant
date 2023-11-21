#pragma once

#include <glm/glm.hpp>

#include <assimp/matrix4x4.h>

namespace Radiant::Math
{
	void DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);
	glm::mat4 Mat4FromAIMatrix4x4(const aiMatrix4x4& matrix);
}