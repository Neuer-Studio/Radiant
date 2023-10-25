#pragma once

#include <glm/glm.hpp>

namespace Radiant::Utils::ImGui
{
	void Image(void* texture, const glm::vec2& size, const glm::vec2& uv0 = glm::vec2(0),
		const glm::vec2& uv1 = glm::vec2(1), const glm::vec4& color = glm::vec4(0), const glm::vec4& bordercolor = glm::vec4(1));

	void Text(const std::string& text = "Unnamed Text");
	void SliderFloat(const std::string& label = "Unnamed Label", float* v = nullptr, float v_min = -5.0f, float v_max = 5.0f, const char* format = nullptr, float power = 1.0f);
	void SliderUint(const std::string& label = "Unnamed Label", int* v = nullptr, float v_max = 10, const char* format = nullptr, float power = 1.0f);
}