#pragma once

#include <string>

namespace Radiant
{
	class Layer
	{
	public:
		Layer(const std::string& name = "DebugName");
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}

		inline const std::string& GetName() { return m_Name; }
	private:
		std::string m_Name;
	};
}