#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

void
arbint_set_zero(arbint* to_reset)
{
	for (size_t i = 0; i < to_reset->length; i++)
	{
		to_reset->value[i] = 0;
	}
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

void
print_arbint(arbint* to_print)
{
	// Print a representation of an arbint struct for debugging.

	// If it's a null pointer we don't have much to do
	if (to_print == NULL)
	{
		printf("arbint a = NULL;\n");
		return;
	}

	// First we declare the value array
	printf("uint32_t value_array[%lu] = {", to_print->length);
	for (size_t i = 0; i < (to_print->length); i++)
	{
		printf("%u%s",
		       (to_print->value)[i],
		       i + 1 == to_print->length ? "" : ", ");
	}
	printf("};\n");

	// Print length & value
	printf("arbint a = {\n");
	printf("\t.value = value_array,\n");
	printf("\t.length = %lu,\n", to_print->length);

	// Print the sign
	char* sign_fmt_str = "\t.sign = %s\n";
	switch (to_print->sign)
	{
		case NEGATIVE:
			printf(sign_fmt_str, "NEGATIVE");
			break;
		case POSITIVE:
			printf(sign_fmt_str, "POSITIVE");
			break;
		default:
			printf(sign_fmt_str, "< invalid >");
			break;
	}
	printf("};\n");
}
