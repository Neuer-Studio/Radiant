#pragma once

#include "Core/Logger.hpp"

int main()
{
	Radiant::LogInit();
	while (1)
	{
		RA_INFO("HELLO");
	}
	return 0;
}