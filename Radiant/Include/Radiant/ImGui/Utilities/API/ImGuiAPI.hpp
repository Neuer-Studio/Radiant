#pragma once

#include <glm/glm.hpp>

namespace Radiant::Utils::ImGui::API
{
	void Image(void* texture, const glm::vec2& size, const glm::vec2& uv0 = glm::vec2(0),
		const glm::vec2& uv1 = glm::vec2(1), const glm::vec4& color = glm::vec4(0), const glm::vec4& bordercolor = glm::vec4(1));

	void Text(const std::string& text = "Unnamed Text");
}