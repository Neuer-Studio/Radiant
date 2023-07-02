#pragma once

namespace Radiant::Memory
{
	static constexpr uint32_t COMMAND_CAPACITY = 100;

	class CommandBuffer
	{
	public:
		CommandBuffer()
		{
			m_CommandList.reserve(COMMAND_CAPACITY);
		}

		void AddCommand(std::function<void()> command)
		{
			if (m_CommandList.size() >= COMMAND_CAPACITY)
				RADIANT_VERIFY(false);
			m_CommandList.push_back(command);
			m_CommandCount++;
		}
		void Execute()
		{
			for (auto& cmd : m_CommandList)
			{
				cmd();
			}
			m_CommandCount = 0;
			m_CommandList.clear();
		}
	private:
		std::vector<std::function<void()>> m_CommandList;
		uint32_t m_CommandCount = 0;
	};
}