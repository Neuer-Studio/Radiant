#pragma once

namespace Radiant::Utils::ImGui
{
	void SubmitFrame(const std::string& name, std::function<void()> frame);
}