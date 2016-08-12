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

void
arbint_mul(arbint* to_mul, uint32_t multiplier)
{
	// Multiply an arbint by any 32-bit unsigned integer

	if (multiplier == 0)
	{
		// x * 0 = 0
		arbint_set_zero(to_mul);
		return;
	}
	else if (multiplier == 1)
	{
		// x * 1 = x
		return;
	}

	size_t position      = 0;
	uint64_t temp_result = 0;

	// - 1 because instead of (return n*x) we do (x += (x*(n-1)))
	uint64_t multiplier_internal = (uint64_t) multiplier - 1;

	// Compute the result of each digit multiplied by the multiplier and store
	// it in mul_results
	uint64_t* mul_results = calloc(to_mul->length, sizeof(uint64_t));
	for (size_t position = 0; position < to_mul->length; position++)
	{
		// Multiply using 64-bit ints to keep possible overflow
		temp_result = (uint64_t) to_mul->value[position] * multiplier_internal;
		mul_results[position] = temp_result;
	}

	/*
	At this point we have about the following:
	(imagine that 4 bits were 32 bits)

	      #3   #2   #1   #0
	     xxxx xxxx xxxx xxxx // original number
	               xxxx xxxx // digit #0 * (multiplier - 1) \
	          xxxx xxxx      // digit #1 * (multiplier - 1) | stored in
	     xxxx xxxx           // digit #2 * (multiplier - 1) | mul_results
	xxxx xxxx                // digit #3 * (multiplier - 1) /

	Now we need to add all the above rows together. If the most significant
	digit overflowed, add_to_arbint will reallocate to_mul->value to fit an
	extra digit.
	*/

	size_t max_digits = to_mul->length;
	for (position = 0; position < max_digits; position++)
	{
		// Add the lower 32 bits to the value at position
		add_to_arbint(to_mul, (uint32_t) mul_results[position], position);

		// Add the upper 32 bits to the next value
		mul_results[position] >>= 32;
		add_to_arbint(to_mul, (uint32_t) mul_results[position], position + 1);
	}

	free(mul_results);
}

void
arbint_free(arbint* to_free)
{
	free(to_free->value);
	free(to_free);
}

void
arbint_free_static(arbint* to_free)
{
	free(to_free->value);
}

void
arbint_init(arbint* new_arbint)
{
	uint32_t* value_array = calloc(1, sizeof(uint32_t));

	new_arbint->value  = value_array;
	new_arbint->length = 1;
	new_arbint->sign   = POSITIVE;
}

void
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

	arbint_set_zero(to_fill);

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
		if (input_str[0] == '+')
			input_str++;
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
}

void
u64_to_arbint(uint64_t value, arbint* to_fill)
{
	to_fill->sign = POSITIVE;

	uint32_t lower_value  = (uint32_t)(value & 0xFFFFFFFF);
	uint32_t higher_value = (uint32_t)(value >> (sizeof(uint32_t) * CHAR_BIT));

	// If value uses the upper 32 bits, use two digits
	if (higher_value)
	{
		to_fill->value    = calloc(2, sizeof(uint32_t));
		to_fill->length   = 2;
		to_fill->value[0] = lower_value;
		to_fill->value[1] = higher_value;
	}
	// If it only uses the lower 32 bits, use one digit
	else
	{
		to_fill->value    = calloc(1, sizeof(uint32_t));
		to_fill->length   = 1;
		to_fill->value[0] = lower_value;
	}
}

void
arbint_to_str(arbint* to_convert, char** to_fill /*, uint32_t base*/)
{
	// TODO Check if 2 <= base <= 36
	// TODO actually support different bases
	// TODO actually output a string and not a bc program

	// xxxxxxxxx * ((2^32)^y) +
	size_t str_length = to_convert->length * 27;

	*to_fill = calloc(str_length, sizeof(char));

	char* str = *to_fill;
	for (size_t i = 0; i < to_convert->length; i++)
	{
		str += sprintf(str,
		               "%u * ((2^32)^%lu) %s",
		               to_convert->value[i],
		               i,
		               i + 1 == to_convert->length ? "" : " + ");
	}
}
