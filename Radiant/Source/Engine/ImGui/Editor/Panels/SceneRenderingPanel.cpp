#include <Radiant/ImGui/Editor/Panels/SceneRenderingPanel.hpp>
#include <Radiant/ImGui/Utilities/API/ImGuiAPI.hpp>
#include <Radiant/Rendering/RenderingAPI.hpp>
#include <Radiant/Rendering/SceneRendering.hpp>
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

					const auto info = RenderingAPI::GetGraphicsInfo();

					std::string viewport = "Viewport: " + std::to_string(width) + " : " + std::to_string(height);
					std::string graphicInfo = "GPU: " + info.Renderer + "\nVersion: " + info.Version;

					Utils::ImGui::Text(viewport);
					Utils::ImGui::Text(graphicInfo);

					Utils::ImGui::SliderFloat("Exposure", &m_Context->m_Exposure);

					int samples = (int)m_Context->m_SceneRendering->GetSamplesCount();
					float lod = (float)m_Context->m_SceneRendering->GetSkyboxLod();

					Utils::ImGui::SliderUint("Samples", &samples, 16);
					Utils::ImGui::SliderFloat("Skybox Lod", &lod, 0, 11);

					m_Context->m_SceneRendering->SetSampelsCount((uint32_t)samples);
					m_Context->m_SceneRendering->SetSkyboxLod((float)lod);

					if (m_Context->m_SelectedEntity)
					{

					}
				}
			});
	}
}

