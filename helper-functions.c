#include <stdlib.h>
#include <stdio.h>

#include "datatypes.h"

int
sign_to_int(sign enum_sign)
{
	switch (enum_sign)
	{
		case NEGATIVE:
			return -1;
			break;
		case ZERO:
			return 0;
			break;
		case POSITIVE:
			return +1;
			break;
		default:
			fprintf(stderr, "Error converting sign (enum to number)\n");
			exit(1);
	}
}

sign int_to_sign(int int_sign)
{
	switch (int_sign)
	{
		case -1:
			return NEGATIVE;
			break;
		case 0:
			return ZERO;
			break;
		case 1:
			return POSITIVE;
			break;
		default:
			fprintf(stderr, "Error converting sign (int to enum)\n");
			exit(1);
	}
}

int
char_to_digit(char c)
{
	if (c >= '0' && c <= '9')
	{
		return (int) (c - '0');
	}
	else
	{
		return -1;
	}
}

int
addition_will_wrap(uint64_t a, uint64_t b)
{
	return (a + b) < a;
}
