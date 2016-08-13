#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "datatypes.h"
#include "debug.h"
#include "helper-functions.h"

#include "operators.h"

void
add_to_arbint(arbint* to_add, uint32_t value, size_t position)
{
	// Add (value * (2^32) ^ position) to an arbint.
	// This adds value to to_add->value[position], and if there's an overflow,
	// it recursively adds the overflow to the more significant digit.

	if (value == 0)
	{
		// x + 0 = x
		return;
	}

	// If we don't have enough space, reallocate
	if (position >= to_add->length)
	{
		// Enough space so that position + the next digit will be in the array
		size_t new_length   = position + 1;
		uint32_t* new_value = realloc(to_add->value, new_length * sizeof(uint32_t));
		if (new_value == NULL)
		{
			fprintf(stderr, "add_to_arbint: failed to realloc\n");
			exit(12); // ENOMEM cannot allocate memory
		}

		// Set the newly allocated space to zero
		// TODO use memset if faster
		for (size_t i = to_add->length; i < new_length; i++)
		{
			new_value[i] = 0;
		}

		to_add->value  = new_value;
		to_add->length = new_length;
	}

	uint32_t carry = addition_will_wrap(to_add->value[position], value);

	to_add->value[position] += value;

	if (carry)
	{
		// We have to do this recursively because when this is done:
		//   0000 1111 1111 (imagine those 4-bit ints were 32 bit)
		// + 0000 0000 0001
		// = 0001 0000 0000
		// then adding the carry to value[position+1] will itself cause an
		// overflow, which has to be carried over another time
		add_to_arbint(to_add, carry, position + 1);
	}
}

arbint*
arbint_add_positive(arbint* a, arbint* b)
{
	// TODO make a subtraction function and call it here if one of them is negative
	if (a->sign == NEGATIVE || b->sign == NEGATIVE)
	{
		fprintf(stderr, "arbint_add_positive ignores the sign of numbers");
		exit(22);
	}
	if (a->length < b->length)
	{
		return arbint_add_positive(b, a);
	}

	// If we copy the longer one and add the shorter one, adding will be faster
	// and it's less likely that we have to reallocate while adding
	arbint* result = arbint_copy(a);

	for (size_t i = 0; i < b->length; i++)
	{
		add_to_arbint(result, b->value[i], i);
	}

	return result;
}

static bool
arbint_eq_up_to_length(arbint* a, arbint* b, size_t length)
{
	// This function assumes that both a and b have at least the given length.
	// It is intended to be called from arbint_eq, which does the necessary
	// checking.
	for (size_t i = 0; i < length; i++)
	{
		if (a->value[i] != b->value[i])
		{
			return false;
		}
	}
	return true;
}

static bool
arbint_is_zero(arbint* a)
{
	size_t length = a->length;
	for (size_t i = 0; i < length; i++)
	{
		if (a->value[i])
		{
			return false;
		}
	}
	return true;
}

bool
arbint_eq(arbint* a, arbint* b)
{
	// Check two arbints for numerical equality
	// Returns false if different, true if equal
	// If one of the values is uninitialised, the comparison returns false.

	if (a == NULL || b == NULL)
	{
		fprintf(stderr, "arbint_eq: Got null pointer as argument");
		exit(14); // EFAULT bad address
	}

	if (a->value == NULL || b->value == NULL)
	{
		return false;
	}

	// Do some easy checks first based on signs and equality to 0
	// Right now we pass through all values twice (once now while checking
	// for 0 and another time when checking against each other)
	// TODO: find out if optimisation is worth it here
	bool a_is_zero = arbint_is_zero(a);
	bool b_is_zero = arbint_is_zero(b);

	// If both are zero, they're equal
	if (a_is_zero && b_is_zero)
	{
		return true;
	}
	// If one is zero and the other not, then they're definitely different
	else if (a_is_zero != b_is_zero)
	{
		return false;
	}
	// If they're both nonzero, they are different if the signs are different
	// Only if these signs are equal we need to compare the values
	else if (!a_is_zero && !b_is_zero)
	{
		if (a->sign != b->sign)
			return false;
	}
	// TODO find out if there's more we can say based on signs and zeroness

	// If they have a different length
	if (a->length != b->length)
	{
		// Find out which one is shorter and which one is longer
		arbint* shorter;
		arbint* longer;

		if (a->length < b->length)
		{
			shorter = a;
			longer  = b;
		}
		else
		{
			shorter = b;
			longer  = a;
		}

		size_t shorter_length = shorter->length;
		size_t longer_length  = longer->length;

		assert(shorter_length < longer_length);

		// See if we can already spot a difference in the 'digits' that are
		// present in both numbers.
		bool tmp_result = arbint_eq_up_to_length(shorter, longer, shorter_length);

		if (!tmp_result)
		{
			// If yes, we know that the numbers are different from each other
			return false;
		}

		// Otherwise, we need to check the 'digits' that are only present in
		// the longer number. If they are just zeroes, the numbers are indeed
		// equal, otherwise not (because the non-existent values in the shorter
		// arbint are implicitly zero)
		for (size_t i = shorter_length; i < longer_length; i++)
		{
			if (longer->value[i])
			{
				return false;
			}
		}

		// At this point we've checked all digits and they either match, or
		// are zero in one number and don't exist in the other one.
		return true;
	}
	else
	{
		// If they have an equal length, we can just check each 'digit'.
		assert(a->length == b->length);
		return arbint_eq_up_to_length(a, b, a->length);
	}
}

void
arbint_neg(arbint* to_negate)
{
	if (to_negate->sign == POSITIVE)
	{
		to_negate->sign = NEGATIVE;
	}
	else if (to_negate->sign == NEGATIVE)
	{
		to_negate->sign = POSITIVE;
	}
	else
	{
		fprintf(stderr, "arbint_neg: Invalid sign");
	}
}
