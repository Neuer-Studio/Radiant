#pragma once

#include <Radiant/Memory/Buffer.hpp>

#include <Radiant/Rendering/VertexBuffer.hpp>
#include <Radiant/Rendering/Rendering.hpp>
#include <Radiant/Rendering/Shader.hpp>
#include <Radiant/Rendering/Texture.hpp>
#include <Radiant/Rendering/Pipeline.hpp>

#include <Radiant/Rendering/Platform/OpenGL/OpenGLShader.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>
#include <Radiant/Rendering/SceneRendering.hpp>
#include <Radiant/ImGui/Editor/Panels/PanelOutliner.hpp>
#include <Radiant/ImGui/Editor/Panels/SceneRenderingPanel.hpp>

namespace Radiant
{


	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
			: Layer("EditorLayer")
		{}

		~EditorLayer()
		{
			delete m_ManagerScene;
		}

		virtual void OnAttach()
		{
			m_ManagerScene = new SceneManager("Debug name"); //NOTE(Danya): Should be shared 
			m_TestScene = m_ManagerScene->Create("Debug name");
			m_Outliner = new PanelOutliner(m_TestScene);
			m_ScenePanel = new SceneRenderingPanel(m_TestScene);
			/*auto em = m_TestScene->CreateEntity();
			em->AddComponent(CreateNewComponent<MeshComponent>());
			em->GetComponent(ComponentType::Mesh).As<MeshComponent>()->Mesh = (new Mesh("Resources/Meshes/Cube1m.fbx"));*/
			m_Rendering = new SceneRendering(m_TestScene);

		}

		virtual void OnDetach()
		{

		}

		virtual void OnUpdate() override
		{
			m_TestScene->UpdateScene(m_Rendering);
		}

		virtual void OnImGuiRender() override
		{
			// ImGui + Dockspace Setup ------------------------------------------------------------------------------
			ImGuiIO& io = ImGui::GetIO();
			ImGuiStyle& style = ImGui::GetStyle();
			auto boldFont = io.Fonts->Fonts[0];

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || (ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
			{
			}

			io.ConfigWindowsResizeFromEdges = io.BackendFlags & ImGuiBackendFlags_HasMouseCursors;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			bool isMaximized = Application::Get().GetWindow()->IsWindowMaximized();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, isMaximized ? ImVec2(6.0f, 6.0f) : ImVec2(1.0f, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.0f);
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
			ImGui::Begin("DockSpace Demo", nullptr, window_flags);
			ImGui::PopStyleColor(); // MenuBarBg
			ImGui::PopStyleVar(2);

			ImGui::PopStyleVar(2);

			m_Outliner->DrawComponentsUI();

			// Dockspace
			float minWinSizeX = style.WindowMinSize.x;
			style.WindowMinSize.x = 370.0f;
			ImGui::DockSpace(ImGui::GetID("MyDockspace"));
			style.WindowMinSize.x = minWinSizeX;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Viewport");
			{
				auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
				auto viewportSize = ImGui::GetContentRegionAvail();
				m_Rendering->SetSceneVeiwPortSize({viewportSize.x, viewportSize.y});
				if (m_TestScene->ContainsEntityInScene(ComponentType::Camera))
				{
					m_TestScene->GetEntityByComponentType(ComponentType::Camera)->GetComponent(ComponentType::Camera).As<CameraComponent>()->Camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 1000.0f));
				}

				ImGui::Image((void*)m_Rendering->GetFinalPassImage()->GetImageID(), viewportSize, {0, 1}, {1, 0});


				static int counter = 0;
				auto windowSize = ImGui::GetWindowSize();
				ImVec2 minBound = ImGui::GetWindowPos();
				minBound.x += viewportOffset.x;
				minBound.y += viewportOffset.y;

			}

			ImGui::End();
			ImGui::PopStyleVar();

			m_Outliner->DrawImGuiUI();
			m_ScenePanel->DrawImGuiUI();
			ImGui::End();

		}
	private:

		SceneManager* m_ManagerScene;
		Memory::Shared<Scene> m_TestScene;

		Memory::Shared<SceneRenderingPanel> m_ScenePanel;
		Memory::Shared<PanelOutliner> m_Outliner;
		Memory::Shared<SceneRendering> m_Rendering;
	};
}