#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

#include "datatypes.h"

#include "operators.h"


static bool
arbint_eq_up_to_length(arbint* a, arbint* b, size_t length)
{
	// This function assumes that both a and b have at least the given length.
	// It is intended to be called from arbint_eq, which does the necessary checking.
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
	size_t length = a -> length;
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
			longer = b;
		}
		else
		{
			shorter = b;
			longer = a;
		}

		size_t shorter_length = shorter->length;
		size_t longer_length = longer->length;

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

