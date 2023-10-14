#include <Radiant/ImGui/Editor/Panels/SceneRenderingPanel.hpp>
#include <Radiant/ImGui/Utilities/API/ImGuiAPI.hpp>
#include <Radiant/ImGui/Utilities/Utilities.hpp>

namespace Radiant
{
	SceneRenderingPanel::SceneRenderingPanel(const Memory::Shared<Scene>& scene)
		: m_Context(scene)
	{

	}

	void SceneRenderingPanel::DrawImGuiUI()
	{
		Utils::ImGui::SubmitFrame("Scene", [=]()
			{
				if (m_Context)
				{
					auto width = m_Context->m_ViewportWidth;
					auto height = m_Context->m_ViewportHeight;

					std::string viewport = "Viewport: " + std::to_string(width) + " : " + std::to_string(height);

					Utils::ImGui::API::Text(viewport);
				}
			});
	}
}

