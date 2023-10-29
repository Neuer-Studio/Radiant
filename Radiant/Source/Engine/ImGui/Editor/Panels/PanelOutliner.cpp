#include <Radiant/ImGui/Editor/Panels/PanelOutliner.hpp>
#include <Radiant/Scene/Entity.hpp>
#include <Radiant/Scene/Component.hpp>
#include <Radiant/ImGui/Utilities/UI.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Radiant
{
	namespace
	{
		template <typename UIFunction>
		static void DrawComponentUI(ComponentType type, const std::string& name, Entity* entity, UIFunction uiFunction)
		{
			const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
			if (entity->HasComponent(type))
			{
				ImGui::PushID(name.c_str());
				auto& component = entity->GetComponent(type);
				ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				ImGui::Separator();
				bool open = ImGui::TreeNodeEx("##dummy_id", treeNodeFlags, name.c_str());
				ImGui::PopStyleVar();
				ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
				if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
				{
					ImGui::OpenPopup("ComponentSettings");
				}

				bool removeComponent = false;
				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove component"))
						removeComponent = true;

					ImGui::EndPopup();
				}

				if (open)
				{
					uiFunction(component);
					ImGui::TreePop();
				}

				if (removeComponent)
					entity->RemoveComponent(type);

				ImGui::PopID();
			}
		}

		static bool DrawVec3UI(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
		{
			bool modified = false;

			ImGuiIO& io = ImGui::GetIO();
			auto boldFont = io.Fonts->Fonts[0];
			
			ImGui::PushID(label.c_str());

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.3f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.4f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.3f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
			{
				values.x = resetValue;
				modified = true;
			}

			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			modified |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", buttonSize))
			{
				values.y = resetValue;
				modified = true;
			}

			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			modified |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Z", buttonSize))
			{
				values.z = resetValue;
				modified = true;
			}

			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			modified |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::Columns(1);

			ImGui::PopID();

			return modified;
		}

	}

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
			DrawEntityNodeUI(e);

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_Context->m_SelectedEntity)
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
					m_Context->CreateEntity("Empty Entity");
				}

				ImGui::Spacing();

				if (ImGui::MenuItem("Camera"))
				{
					Entity* entity = m_Context->CreateEntity("Camera");
					auto camera = CreateNewComponent<CameraComponent>();
					camera->Camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), (float)1600.f, (float)1600.f, 10.1f, 10000.0f));
					entity->AddComponent(camera);

				}

				ImGui::Spacing();

				if (ImGui::MenuItem("Mesh"))
				{
					Entity* entity = m_Context->CreateEntity("Mesh");
					auto mesh = CreateNewComponent<MeshComponent>();
					entity->AddComponent(mesh);
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
						Entity* entity = m_Context->CreateEntity("SkyLight");
						auto cube = CreateNewComponent<CubeComponent>();
						cube->Cube = TextureCube::Create("Resources/Envorement/Arches_E_PineTree_Irradiance.tga");
						entity->AddComponent(cube);
					}

					ImGui::Spacing();

					if (ImGui::MenuItem("Directional Light"))
					{
						Entity* entity = m_Context->CreateEntity("Directional Light");
						auto light = CreateNewComponent<DirectionLightComponent>();
						entity->AddComponent(light);
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

		ImGuiTreeNodeFlags flags = (entity == m_Context->m_SelectedEntity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)entity, flags, name);

		if (ImGui::IsItemClicked())
		{
			m_Context->m_SelectedEntity = entity;
		}

		if (opened)
		{
			// TODO: Children
			ImGui::TreePop();
		}
	}

	void PanelOutliner::DrawPropertiesUI()
	{
		Entity* entity = m_Context->m_SelectedEntity;
		ImGui::AlignTextToFramePadding();
		// ...
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		auto& name = m_Context->m_SelectedEntity->m_Name;
		if (m_Context->m_SelectedEntity->m_Name.empty())
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

		std::string TextUUID("UUID: " + m_Context->m_SelectedEntity->m_UUID.ToString());

		ImGui::Text(TextUUID.c_str());
		ImGui::Unindent(contentRegionAvailable.x * 0.05f);
		ImGui::PopItemWidth();


		DrawComponentUI(ComponentType::Transform, "Transform", entity, [=](Memory::Shared<Component>& component) mutable
			{
				DrawVec3UI("Translation", component.As<TransformComponent>()->Translation);
				glm::vec3 rotation = glm::degrees(component.As<TransformComponent>()->Rotation);
				DrawVec3UI("Rotation", rotation);
				component.As<TransformComponent>()->Rotation = glm::radians(rotation);
				DrawVec3UI("Scale", component.As<TransformComponent>()->Scale, 1.0f);
			});

		DrawComponentUI(ComponentType::Mesh, "Mesh", entity, [=](Memory::Shared<Component>& component) mutable
			{
				auto& mesh = component.As<MeshComponent>()->Mesh;
				UI::BeginPropertyGrid();

				ImGui::Text(entity->GetName().c_str());
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);

				ImVec2 originalButtonTextAlign = ImGui::GetStyle().ButtonTextAlign;
				ImGui::GetStyle().ButtonTextAlign = { 0.0f, 0.5f };
				float width = ImGui::GetContentRegionAvail().x - 0.0f;
				UI::PushID();

				float itemHeight = 28.0f;

				std::string buttonName;
				if (mesh)
					buttonName = mesh->GetName();
				else
					buttonName = "Null";

				if (ImGui::Button(buttonName.c_str(), { width, itemHeight }))
				{
					std::string file = Utils::FileSystem::OpenFileDialog().string();
					if (!file.empty())
					{
						mesh = Memory::Shared<Mesh>::Create(file);

						auto& material = mesh->GetMaterial();
						auto materialComponent = CreateNewComponent<MaterialComponent>(material);
						entity->AddComponent(materialComponent);
					}
				}

				UI::PopID();
				ImGui::GetStyle().ButtonTextAlign = originalButtonTextAlign;
				ImGui::PopItemWidth();

				UI::EndPropertyGrid();

				UI::BeginPropertyGrid();
				UI::EndPropertyGrid();
			});

		DrawComponentUI(ComponentType::DirectionLight, "DirectionLight", entity, [=](Memory::Shared<Component>& component) mutable
			{
				DrawVec3UI("Direction", component.As<DirectionLightComponent>()->DirLight.Direction);
				DrawVec3UI("Radiance", component.As<DirectionLightComponent>()->DirLight.Radiance);
			});


		DrawComponentUI(ComponentType::Material, "Material", entity, [=](Memory::Shared<Component>& component) mutable
			{
				auto a = component.As<MaterialComponent>()->Material->GetOverridedValuesSize();
				ImGui::Text("Test Material %u", component.As<MaterialComponent>()->Material->GetOverridedValuesSize());
			});

		/* New Component */

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