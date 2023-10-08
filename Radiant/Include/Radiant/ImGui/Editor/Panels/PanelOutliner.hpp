#pragma once

#include <Radiant/Scene/Scene.hpp>

namespace Radiant
{
	class PanelOutliner
	{
	public:
		PanelOutliner(const Memory::Shared<Scene>& scene = nullptr);
		void SetContext(const Memory::Shared<Scene>& scene);

		void DrawComponentsUI();
		void DrawImGuiUI();

	private:
		Memory::Shared<Scene> m_Context;
	};
}
