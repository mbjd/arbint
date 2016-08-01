#include <stdlib.h>
#include <stdio.h>

#include "datatypes.h"

#include "helper-functions.h"


int
sign_to_int(sign enum_sign)
{
	switch (enum_sign)
	{
		case NEGATIVE:
			return -1;
			break;
		case POSITIVE:
			return +1;
			break;
		default:
			fprintf(stderr, "Error converting sign (enum to number)\n");
			exit(1);
	}
}

sign
int_to_sign(int int_sign)
{
	switch (int_sign)
	{
		case -1:
			return NEGATIVE;
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

void
print_arbint(arbint* to_print)
{
	// Print a representation of an arbint struct for debugging.
	printf("{\n");
	if (to_print == NULL)
	{
		// Print unallocated struct
		printf("\t<null pointer>");
	}
	else
	{
		// Print length & value
		if (to_print -> length)
		{
			printf("\tlength: %d\n", to_print -> length);
			printf("\tvalue: [\n");
			for (int i = 0; i < (to_print -> length); i++)
			{
				printf("\t\t%d: %lu\n", i, (to_print -> value)[i]);
			}
			printf("\t]\n");
		}
		else
		{
			printf("\tlength: 0\n");
		}

		switch (to_print -> sign)
		{
			case NEGATIVE:
				printf("\tsign: NEGATIVE\n");
				break;
			case POSITIVE:
				printf("\tsign: POSITIVE\n");
				break;
			default:
				printf("\tsign: <undefined>\n");
				break;
		}
	}
	printf("}\n");
}
