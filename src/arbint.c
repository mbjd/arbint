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
add_to_arbint(arbint* to_add, uint32_t value, uint32_t position)
{
	// Add (value * (2^64) ^ position) to an arbint.

	// If we don't have enough space, reallocate
	if (position >= to_add->length)
	{
		// Enough space so that position + the next digit will be in the array
		size_t new_length = 2 + position;
		to_add->value = realloc(to_add->value, new_length * sizeof(uint32_t));

		if (to_add->value == NULL)
		{
			fprintf(stderr, "add_to_arbint: Not enough memory for realloc\n");
			exit(12);
		}

		to_add->length = new_length;
	}

	uint32_t carry =
	    (uint32_t) addition_will_wrap(to_add->value[position], value);

	to_add->value[position] += value;

	if (carry)
	{
		// We have to do this recursively because when this is done:
		//   0000 1111 1111 (imagine those 4-bit ints were 32 bit)
		// + 0000 0000 0001
		// = 0001 0000 0000
		// then adding the carry to value[position+1] will itself cause an
		// overflow, which has to be carried over another time
		// TODO in order for this to work, we have to make sure the most
		// significant number in bigint.value is always 0
		add_to_arbint(to_add, carry, position + 1);
	}
	return;
}

static void
arbint_mul(arbint* to_mul, uint32_t multiplier)
{
	// Multiply an arbint by any 32-bit unsigned integer

	if (multiplier == 0)
	{
		// When multiplying by 0, clear everything out
		for (size_t i = 0; i < to_mul->length; i++)
		{
			to_mul->value[i] = 0;
		}
		return;
	}
	else if (multiplier == 1)
	{
		// Do nothing
		return;
	}

	size_t position              = 0;
	uint64_t temp_result         = 0;
	uint64_t multiplier_internal = (uint64_t) multiplier;

	while (position < to_mul->length)
	{
		// Multiply with 64-bit ints to keep possible overflow
		temp_result = (uint64_t) to_mul->value[position] * multiplier_internal;
		// Masking out may be unnecessary when casting to a smaller uint
		to_mul->value[position] = (uint32_t)(temp_result & (UINT32_MAX));

		// Shift to the right to get the overflown part
		temp_result >>= 32;
		if (temp_result)
		{
			// Add the overflown part to the next digit
			add_to_arbint(to_mul, (uint32_t) temp_result, position + 1);
		}
		position++;
	}
}

void
arbint_free(arbint* to_free)
{
	free(to_free->value);
	free(to_free);
}

void
arbint_init(arbint* new_arbint)
{
	uint32_t* value_array = calloc(1, sizeof(uint32_t));

	new_arbint->value  = value_array;
	new_arbint->length = 1;
	new_arbint->sign   = POSITIVE;
}

arbint*
str_to_arbint(char* input_str, arbint* to_fill, uint32_t base)
{
	if (base < 2)
	{
		fprintf(stderr, "str_to_arbint: Base must be between 2 and 36\n");
	}
	else if (base > 36)
	{
		fprintf(stderr, "str_to_arbint: Base must be between 2 and 36\n");
	}
	// TODO: Trim whitespace

	// Get the sign
	// Now we can be sure that to_fill points to something
	if (input_str[0] == '-')
	{
		to_fill->sign = NEGATIVE;
		input_str++;
	}
	else
	{
		to_fill->sign = POSITIVE;
	}

	size_t position = 0;

	// Iterate over the string, starting at the most significant digit. Every
	// time a digit is read, we multiply the accumulated value in to_fill by
	// 10 and move to the next digit
	while (input_str[position])
	{
		int digit_val = char_to_digit(input_str[position], base);

		if (digit_val == -1)
		{
			fprintf(stderr,
			        "str_to_arbint: Invalid character '%c' for base %d",
			        input_str[position],
			        base);
			exit(EINVAL); // 22 Invalid argument
		}

		// - Add the digit to the arbint
		// - Multiply the arbint by 10
		add_to_arbint(to_fill, digit_val, 0);
		if (input_str[position + 1])
		{
			arbint_mul(to_fill, base);
		}

		position++;
	}

	// TODO finish this function. Right now you have to set the
	// attributes of each arbint struct manually, which is shit

	// TODO don't return address of local variable (maybe pass
	// an empty struct as an argument)
	return to_fill;
}
