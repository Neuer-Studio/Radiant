#include <Core/LayerStack.hpp>

namespace Radiant
{
	LayerStack::LayerStack()
	{

	}

	LayerStack::~LayerStack()
	{

	}

	void LayerStack::PushLayer(Layer* layer)
	{
		RA_INFO("[Layer] Adding Layer {}", layer->GetName());
		m_Layers.push_back(layer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = m_Layers.begin();
		while (it != m_Layers.end()) {
			if (*it == layer)
			{
				m_Layers.erase(it);
				break;
			}
			++it;
		}
	}
}