#include <Radiant/ImGui/Utilities/API/ImGuiAPI.hpp>

#include <ImGui/imgui.h>

namespace Radiant::Utils::ImGui::API
{
	void Image(void* texture, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec4& color, const glm::vec4& bordercolor)
	{
		::ImGui::Image(texture, { size.x, size.y }, { uv0.x, uv0.y }, { uv1.x, uv1.y }, { color.r, color.g, color.b, color.a }, { bordercolor.r, bordercolor.g, bordercolor.b, bordercolor.a });
	}

	void Text(const std::string& text)
	{
		::ImGui::Text(text.c_str());
	}
}