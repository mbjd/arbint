#pragma once

#include <stdio.h>

#define DEBUG 1
#define PRINT_DEBUG(str)                    \
	if (DEBUG)                          \
	{                                   \
		printf("DEBUG: %s\n", str); \
	}
