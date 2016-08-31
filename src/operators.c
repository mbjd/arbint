#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "arbint.h"
#include "datatypes.h"
#include "debug.h"
#include "helper-functions.h"

#include "operators.h"

// Forward declarations for internal functions
static int arbint_cmp_with_sign(arbint a, arbint b, sign a_sign, sign b_sign);
static arbint arbint_sub_with_sign(arbint a, arbint b, sign a_sign, sign b_sign);
void add_to_arbint(arbint to_add, uint32_t value, size_t position);

void
add_to_arbint(arbint to_add, uint32_t value, size_t position)
{
	// Add (value * (2^32) ^ position) to an arbint.
	// This adds value to to_add->value[position], and if there's an overflow,
	// it recursively adds the overflow to the more significant digit.
	// If the overflow goes beyond the length of to_add, its value is reallocated
	// to fit the new value.

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

arbint
arbint_add_primitive(arbint a, arbint b)
{
	// This ignores the signs and assumes that both are positive!
	// The return value is always positive
	if (a->length < b->length)
	{
		return arbint_add_primitive(b, a);
	}

	// If we copy the longer one and add the shorter one, adding will be faster
	// and it's less likely that we have to reallocate while adding
	arbint result = arbint_copy(a);
	result->sign  = POSITIVE;

	for (size_t i = 0; i < b->length; i++)
	{
		add_to_arbint(result, b->value[i], i);
	}

	return result;
}

arbint
arbint_add(arbint a, arbint b)
{
	bool a_is_zero = arbint_is_zero(a);
	bool b_is_zero = arbint_is_zero(b);

	if (a_is_zero && b_is_zero)
		return arbint_new();

	if (a_is_zero)
		return arbint_copy(b);
	if (b_is_zero)
		return arbint_copy(a);

	if (a->sign == POSITIVE)
	{
		if (b->sign == POSITIVE)
		{
			// Both are positive -> we can just add
			return arbint_add_primitive(a, b);
		}
		else // if (b->sign == NEGATIVE)
		{
			// This is a subtraction, let's return a - abs(b)
			return arbint_sub_with_sign(a, b, POSITIVE, POSITIVE);
		}
	}
	else // if (a->sign == NEGATIVE)
	{
		if (b->sign == POSITIVE)
		{
			// This is a subtraction, let's return b - abs(a)
			return arbint_sub_with_sign(b, a, POSITIVE, POSITIVE);
		}
		else // if (b->sign == NEGATIVE)
		{
			// Both are negative! We can calculate abs(a) + abs(b) and then
			// flip the sign
			arbint result = arbint_add_primitive(a, b);
			arbint_neg(result);
			return result;
		}
	}
}

static arbint
arbint_sub_primitive(arbint a, arbint b)
{
	// Return the result of a - b
	// This assumes that both a and b are positive and that a >= b.

	arbint result = arbint_new_length(a->length);

	uint32_t digit_result;
	uint32_t borrow = 0;

	for (size_t i = 0; i < a->length; i++)
	{
		// Do the subtraction
		digit_result = a->value[i] - b->value[i];

		// Handle borrow from previous iteration
		if (borrow)
		{
			digit_result -= borrow;
			borrow = 0;
		}

		result->value[i] = digit_result;

		// Detect wrapping
		if (digit_result > a->value[i])
		{
			// Wrapped
			// Keep the result anyway, but subtract one from
			// the next higher digit (TODO check if correct)
			borrow = 1;
		}
	}

	return result;
}

arbint
arbint_sub_with_sign(arbint a, arbint b, sign a_sign, sign b_sign)
{
	if (a == b)
	{
		return arbint_new();
	}

	bool a_is_zero = arbint_is_zero(a);
	bool b_is_zero = arbint_is_zero(b);

	if (a_is_zero && b_is_zero)
	{
		return arbint_new();
	}
	else if (a_is_zero)
	{
		// 0 - b = -b
		arbint result = arbint_copy(b);
		arbint_neg(result);
		return result;
	}
	else if (b_is_zero)
	{
		// a - 0 = a
		return arbint_copy(a);
	}

	int cmp = arbint_cmp_with_sign(a, b, a_sign, b_sign);

	if (cmp == 0)
	{
		return arbint_new();
	}

	if (a_sign == POSITIVE)
	{
		if (b_sign == POSITIVE)
		{
			// Both numbers are positive
			if (cmp == +1)
			{
				// a > b, so the result is positive
				return arbint_sub_primitive(a, b);
			}
			else // if (cmp == -1)
			{
				// a < b, so the result is negative
				// a - b = c    | * -1
				// b - a = -c   --> -c > 0
				// So we need to calculate -(b - a) to get a - b
				arbint result = arbint_sub_primitive(b, a);
				result->sign  = NEGATIVE;
				return result;
			}
		}
		else // if (b_sign == NEGATIVE)
		{
			// a > 0, b < 0
			// a - b = a + abs(b) if b < 0
			return arbint_add_primitive(a, b);
		}
	}
	else // if (a_sign == NEGATIVE)
	{
		if (b_sign == POSITIVE)
		{
			// a < 0, b > 0
			// a - b = -(-a + b)
			arbint result = arbint_add_primitive(a, b);
			result->sign  = NEGATIVE;
			return result;
		}
		else // if (b->sign == NEGATIVE)
		{
			// Both numbers are negative
			if (cmp == +1)
			{
				// 0 > a > b
				// -a - (-b) = c
				// - (a - b) = c
				// b - a = c
				return arbint_sub_primitive(b, a);
			}
			else // if (cmp == -1)
			{
				// 0 > b > a
				// -a - (-b) = c
				// - (a - b) = c
				arbint r = arbint_sub_primitive(a, b);
				r->sign  = NEGATIVE;
				return r;
			}
		}
	}
}

arbint
arbint_sub(arbint a, arbint b)
{
	return arbint_sub_with_sign(a, b, a->sign, b->sign);
}

static bool
arbint_eq_up_to_length(arbint a, arbint b, size_t length)
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

bool
arbint_is_zero(arbint a)
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
arbint_eq(arbint a, arbint b)
{
	// Check two arbints for numerical equality
	// Returns false if different, true if equal
	// If one of the values is uninitialised, the comparison returns false.
	// Could also be implemented as arbint_cmp(a, b) == 0 but that would be a
	// bit less
	// efficient since you'd have to check the whole number.

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
		arbint shorter;
		arbint longer;

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

		// See if we can already spot a difference in the 'digits' that
		// are
		// present in both numbers.
		bool tmp_result = arbint_eq_up_to_length(shorter, longer, shorter_length);

		if (!tmp_result)
		{
			// If yes, we know that the numbers are different from
			// each other
			return false;
		}

		// Otherwise, we need to check the 'digits' that are only present
		// in
		// the longer number. If they are just zeroes, the numbers are
		// indeed
		// equal, otherwise not (because the non-existent values in the
		// shorter
		// arbint are implicitly zero)
		for (size_t i = shorter_length; i < longer_length; i++)
		{
			if (longer->value[i])
			{
				return false;
			}
		}

		// At this point we've checked all digits and they either match,
		// or
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

static int
arbint_cmp_with_sign(arbint a, arbint b, sign a_sign, sign b_sign)
{
	// Returns +1 if a > b, 0 if a == b, -1 if a < b
	// a_sign and b_sign override the actual signs a->sign and b->sign
	// If you just want to use those signs, use arbint_cmp()

	if (a == b)
		return 0; // If it's the exact same pointer

	bool a_is_zero = arbint_is_zero(a);
	bool b_is_zero = arbint_is_zero(b);

	if (a_is_zero && b_is_zero)
		return 0;

	if (a_is_zero)
	{
		if (b_sign == POSITIVE)
			return -1;
		else
			return +1;
	}

	if (b_is_zero)
	{
		if (b_sign == POSITIVE)
			return +1;
		else
			return -1;
	}

	// At this point we can be sure that none of the two arbints are 0
	// This means that if signs are different the numbers are too
	if (a_sign != b_sign)
	{
		if (a_sign == POSITIVE)
			return +1;
		else
			return -1;
	}

	// Now the sign of the numbers is equal and they are both != 0.
	// This means we need to actually check the value.
	assert(a_sign == b_sign);
	size_t a_highest_digit = arbint_highest_digit(a);
	size_t b_highest_digit = arbint_highest_digit(b);

	// Instead of immeiately returning, we set this value and goto finish.
	// There we need to reverse the result if the values are negative, because
	// we want to compare a to b, not abs(a) to abs(b).
	int retval;

	if (a_highest_digit > b_highest_digit)
	{
		retval = +1;
		goto finish;
	}
	if (a_highest_digit < b_highest_digit)
	{
		retval = -1;
		goto finish;
	}

	// Now we know that they both have the same amount of digits
	// (ignoring leading zeroes)
	assert(a_highest_digit == b_highest_digit);
	size_t position = a_highest_digit;

	// Find the highest digit that is different in both numbers
	while (a->value[position] == b->value[position] && position)
		position--;

	if (a->value[position] > b->value[position])
	{
		retval = +1;
		goto finish;
	}
	else if (a->value[position] < b->value[position])
	{
		retval = -1;
		goto finish;
	}
	else
	{
		// In this case we must be at 0 because of the while loop
		// above this
		assert(position == 0);
		return 0;
	}

finish:
	// If the numbers are both negative, the results are exactly opposite.
	// Only check one sign because they're both equal anyway
	if (a_sign == NEGATIVE)
	{
		retval *= -1;
	}
	return retval;
}

int
arbint_cmp(arbint a, arbint b)
{
	return arbint_cmp_with_sign(a, b, a->sign, b->sign);
}

bool
arbint_lt(arbint a, arbint b)
{
	// true if a < b, false otherwise
	return (arbint_cmp(a, b) == -1);
}

bool
arbint_gt(arbint a, arbint b)
{
	// True if a > b, false otherwise
	return (arbint_cmp(a, b) == 1);
}

bool
arbint_leq(arbint a, arbint b)
{
	// True if a <= b, false otherwise
	return (arbint_cmp(a, b) != 1);
}

bool
arbint_geq(arbint a, arbint b)
{
	// True if a >= b, false otherwise
	return (arbint_cmp(a, b) != -1);
}

void
arbint_neg(arbint to_negate)
{
	if (to_negate->sign == POSITIVE)
		to_negate->sign = NEGATIVE;
	else if (to_negate->sign == NEGATIVE)
		to_negate->sign = POSITIVE;
	else
		fprintf(stderr, "arbint_neg: Invalid sign");
}
