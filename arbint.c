#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "datatypes.h"
#include "operators.h"
#include "helper-functions.h"

#include "arbint.h"


void
add_binary_to_arbint(arbint bigint, int64_t value, int position)
{
	// Add (value * (2^64) ^ position) to a arbint.

	uint64_t carry = (uint64_t) addition_will_wrap(bigint.value[position], value);
	bigint.value[position] += value;

	if (carry)
	{
		// We have to do this recursively because when the following operation is done:
		//   0000 1111 1111 (imagine those 4-bit ints were 64 bit)
		// + 0000 0000 0001
		// = 0001 0000 0000
		// then adding the carry to value[position+1] will itself cause an overflow,
		// which has to be carried over another time
		// TODO in order for this to work, we have to make sure the most significant
		// number in bigint.value is always 0
		add_binary_to_arbint(bigint, carry, position + 1);
	}
}


void free_arbint_struct(arbint* to_free)
{
	free(to_free -> value);
	free(to_free);
}

arbint* str_to_arbint(char* input_str)
{
	// TODO: Trim whitespace

	// Get the sign
	int sign = 1;
	if (input_str[0] == '-')
	{
		sign = -1;
		input_str++;
	}

	int position = strlen(input_str) - 1;
	int digit_value = 1;
	int digit = 0;
	int base = 10; // TODO maybe accept other bases too?
	arbint result;

	// Get the fucking value already you lazy ass function
	while (position)
	{
		digit = char_to_digit(input_str[position]);
		if (digit == -1)
		{
			fprintf(stderr, "str_to_arbint: The input string contains an "
			        "invalid character: '%c'", input_str[position]);
			exit(1);
		}
		position -= 1;
	}

	// TODO ...
	return &result;
}
