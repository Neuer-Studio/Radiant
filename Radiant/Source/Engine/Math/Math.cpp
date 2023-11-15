#include <Radiant/Math/Math.hpp>

#include <glm/gtx/matrix_decompose.hpp>

namespace Radiant::Math
{
	void DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(transform, scale, rotation, translation, skew, perspective);
	}
}