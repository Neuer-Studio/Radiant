#include <Radiant/ImGui/Editor/Panels/PanelOutliner.hpp>
#include <imgui/imgui.h>
#include <Radiant/Scene/Entity.hpp>
#include <Radiant/Scene/Component.hpp>

namespace Radiant
{
	PanelOutliner::PanelOutliner(const Memory::Shared<Scene>& context)
		: m_Context(context), m_SelectedEntity(nullptr)
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
			DrawEntityNodeUI(e);

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectedEntity)
			DrawPropertiesUI();

		ImGui::End();
	}

	void PanelOutliner::DrawComponentsUI(const std::string& ButtonName, float x, float y)
	{
		if (m_Context)
		{
			ImGui::Indent(20);
			if (ImGui::Button(ButtonName.c_str(), ImVec2(x,y)))
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

	void PanelOutliner::DrawEntityNodeUI(Entity* entity)
	{
		const char* name = "Unnamed Entity";

		if (!entity->GetName().empty())	
			name = entity->GetName().c_str();

		ImGuiTreeNodeFlags flags = (entity == m_SelectedEntity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)entity, flags, name);

		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		if (opened)
		{
			// TODO: Children
			ImGui::TreePop();
		}
	}

	void PanelOutliner::DrawPropertiesUI()
	{
		ImGui::AlignTextToFramePadding();
		// ...
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		auto& name = m_SelectedEntity->m_Name;
		if (m_SelectedEntity->m_Name.empty())
			name = "Unnamed Entity";
		char buffer[256];
		memset(buffer, 0, 256);
		memcpy(buffer, name.c_str(), name.length());
		ImGui::Indent(contentRegionAvailable.x * 0.05f);
		ImGui::PushItemWidth(contentRegionAvailable.x * 0.9f);
		if (ImGui::InputText("##Tag", buffer, 256))
		{
			name = std::string(buffer);
		}

		std::string TextUUID("UUID: " + m_SelectedEntity->m_UUID.ToString());

		ImGui::Text(TextUUID.c_str());
		ImGui::Unindent(contentRegionAvailable.x * 0.05f);
		ImGui::PopItemWidth();

		ImGui::Dummy(ImVec2(0, 20));

		float buttonWidth = 150.0f;
		float buttonHeight = 50.0f;

		float indentX = (contentRegionAvailable.x - buttonWidth * 1.5) * 0.5f;
		if (indentX > 0)
			ImGui::Indent(indentX);

		DrawComponentsUI("New Component", buttonWidth, buttonHeight);

		if (indentX > 0)
			ImGui::Unindent(indentX);

	}
}