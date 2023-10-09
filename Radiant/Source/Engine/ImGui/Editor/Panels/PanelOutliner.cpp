#include <Radiant/ImGui/Editor/Panels/PanelOutliner.hpp>
#include <imgui/imgui.h>
#include <Radiant/Scene/Entity.hpp>
#include <Radiant/Scene/Component.hpp>

namespace Radiant
{
	PanelOutliner::PanelOutliner(const Memory::Shared<Scene>& context)
		: m_Context(context)
	{
	}

	void PanelOutliner::SetContext(const Memory::Shared<Scene>& scene)
	{
		m_Context = scene;
	}

	void PanelOutliner::DrawImGuiUI()
	{
		ImGui::Begin("Outliner");

		auto& sceneEntities = m_Context->GetEntityList();
		for (const auto& e : sceneEntities)
			ImGui::Button(e->m_Name.c_str());

		ImGui::End();
	}

	void PanelOutliner::DrawComponentsUI()
	{
		if (m_Context)
		{
			ImGui::Indent(20);
			if (ImGui::Button("Add"))
			{
				ImGui::OpenPopup("AddEntityMenu");
			}
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::Unindent(20);

			if (ImGui::BeginPopup("AddEntityMenu"))
			{
				ImGui::Indent(20.0f);

				if (ImGui::MenuItem("Empty Entity"))
				{
					Entity* newEntity = m_Context->CreateEntity("Empty Entity");
				}

				ImGui::Spacing();

				if (ImGui::MenuItem("Mesh"))
				{
				}

				ImGui::Spacing();

				if (ImGui::BeginMenu("3D"))
				{
					if (ImGui::Button("Sphere Collider"))
					{
					}

					ImGui::EndMenu();
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::BeginMenu("Light"))
				{
					if (ImGui::MenuItem("Sky Light"))
					{
					}

					ImGui::Spacing();

					if (ImGui::MenuItem("Directional Light"))
					{
					}

					ImGui::Spacing();

					if (ImGui::MenuItem("Point Light"))
					{
					}

					ImGui::EndMenu();
				}

				ImGui::Unindent();
				ImGui::EndPopup();
			}
		}
	}
}