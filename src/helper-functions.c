#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arbint.h"
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
			exit(EINVAL);
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
			exit(EINVAL);
	}
}

int
char_to_digit(char c, uint32_t base)
{
	int digit;
	if (c >= '0' && c <= '9')
	{
		digit = (int) (c - '0');
	}
	else if (c >= 'a' && c <= 'z')
	{
		digit = (int) (c - 'a') + 10;
	}
	else if (c >= 'A' && c <= 'Z')
	{
		digit = (int) (c - 'A') + 10;
	}
	else
	{
		return -1;
	}

	// At this point digit can't be negative so casting to unsigned is OK
	if ((uint8_t) digit >= base)
	{
		return -1;
	}

	return digit;
}

bool
is_digit(const char c)
{
	return (c >= '0') && (c <= '9');
}

bool
addition_will_wrap(uint32_t a, uint32_t b)
{
	return (a + b) < a;
}

arbint
arbint_copy(arbint src)
{
	// Allocate struct
	arbint dest = calloc(1, sizeof(arbint_struct));

	// Copy the value over
	size_t bytes_to_copy = src->length * sizeof(uint32_t);
	uint32_t* dest_value = malloc(bytes_to_copy);
	if (dest_value == NULL)
	{
		fprintf(stderr, "arbint_copy: malloc returned null\n");
		exit(ENOMEM);
	}
	else
	{
		memcpy(dest_value, src->value, bytes_to_copy);
	}

	dest->value = dest_value;

	// Copy the rest
	dest->sign   = src->sign;
	dest->length = src->length;

	return dest;
}

size_t
arbint_highest_digit(arbint input)
{
	// Returns the position of the most significant non-zero digit
	size_t position = input->length - 1;
	while (input->value[position] == 0)
		position--;
	return position;
}

void
arbint_trim(arbint to_trim)
{
	// Remove all leading zeroes
	size_t last_leading_zero = 1 + arbint_highest_digit(to_trim);
	size_t bytes_to_keep     = last_leading_zero * sizeof(uint32_t);
	uint32_t* new_value      = realloc(to_trim->value, bytes_to_keep);
	if (new_value)
	{
		to_trim->value  = new_value;
		to_trim->length = last_leading_zero;
	}
	else
	{
		fprintf(stderr, "Failed to realloc in arbint_trim\n");
		exit(ENOMEM);
	}
}

void
print_arbint(arbint to_print)
{
	if (to_print == NULL)
	{
		printf("arbint a = NULL;\n");
		return;
	}
	else
	{
		printf("arbint a = arbint_new();\n");

		char** result = calloc(1, sizeof(char*));
		arbint_to_hex(to_print, result);
		printf("str_to_arbint(\"%s\", a, 16);\n", *result);
		free(*result);
		free(result);
	}
}

void
print_arbint_verbose(arbint to_print)
{
	if (to_print == NULL)
	{
		printf("arbint a = NULL;\n");
		return;
	}
	else
	{
		printf("arbint a = arbint_new_length(%lu);\n", to_print->length);
		for (size_t i = 0; i < to_print->length; i++)
		{
			printf("a->value[%lu] = %u;\n", i, to_print->value[i]);
		}
		printf("a->sign = ");
		switch (to_print->sign)
		{
			case POSITIVE:
				puts("POSITIVE;");
				break;
			case NEGATIVE:
				puts("NEGATIVE;");
				break;
			default:
				printf("%d; // invalid sign\n", to_print->sign);
				break;
		}
	}
}
