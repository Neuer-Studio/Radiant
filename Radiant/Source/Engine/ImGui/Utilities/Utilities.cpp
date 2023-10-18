#include <Radiant/ImGui/Utilities/Utilities.hpp>

#include <imgui/imgui.h>

namespace Radiant::Utils::ImGui
{
	void SubmitFrame(const std::string& name, std::function<void()> frame)
	{
		::ImGui::Begin(name.c_str());
		frame();
		::ImGui::End();
	}
}