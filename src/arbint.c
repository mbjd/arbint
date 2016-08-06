#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "datatypes.h"
#include "helper-functions.h"
#include "operators.h"

#include "arbint.h"

static void
add_binary_to_arbint(arbint* bigint, uint64_t value, uint64_t position)
{
	// Add (value * (2^64) ^ position) to an arbint.

	uint64_t carry =
		(uint64_t) addition_will_wrap(bigint->value[position], value);
	bigint->value[position] += value;

	if (carry)
	{
		// We have to do this recursively because when this is done:
		//   0000 1111 1111 (imagine those 4-bit ints were 64 bit)
		// + 0000 0000 0001
		// = 0001 0000 0000
		// then adding the carry to value[position+1] will itself cause an
		// overflow, which has to be carried over another time
		// TODO in order for this to work, we have to make sure the most
		// significant number in bigint.value is always 0
		add_binary_to_arbint(bigint, carry, position + 1);
	}
}

void
free_arbint_struct(arbint* to_free)
{
	free(to_free->value);
	free(to_free);
}

arbint*
str_to_arbint(char* input_str, arbint* to_fill)
{
	// TODO: Trim whitespace

	// Get the sign
	int sign = 1;
	if (input_str[0] == '-')
	{
		sign = -1;
		input_str++;
	}

	int input_length = strlen(input_str);

	// Make sure we only have numbers in the string
	size_t pos = 0;
	while (input_str[pos] != '\0')
	{
		if (!is_digit(input_str[pos]))
		{
			fprintf(stderr,
					"The input string contains non-numeric characters.");
			exit(1);
		}
		pos++;
	}

	// Convert to arbint
	// If a = log10(input), the length of the number in base 2^64 will
	// be a / (log10(2^64)) = a / (log10(2) * 64) = a / 19.265
	size_t arbint_length = 2 + (((double) input_length) / 19.265);

	// Allocate that amount of space
	to_fill->value = calloc(arbint_length, sizeof(uint64_t));

	// TODO finish this function. Right now you have to set the
	// attributes of each arbint struct manually, which is shit

	// TODO don't return address of local variable (maybe pass
	// an empty struct as an argument)
	return to_fill;
}
