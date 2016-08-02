#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "minunit.h"

#include "arbint.h"
#include "operators.h"


int tests_run = 0;

static char*
test_char_to_digit()
{
	mu_assert("char_to_digit('0') == 0", char_to_digit('0') == 0);
	mu_assert("char_to_digit('9') == 9", char_to_digit('9') == 9);
	mu_assert("char_to_digit('*') == -1", char_to_digit('*') == -1);
	mu_assert("char_to_digit('a') == -1", char_to_digit('a') == -1);
	mu_assert("char_to_digit('\\0') == -1", char_to_digit('\0') == -1);
	return 0;
}

static char*
test_sign_to_int()
{
	int neg = sign_to_int(NEGATIVE);
	int pos = sign_to_int(POSITIVE);

	mu_assert("NEGATIVE != -1", neg == -1);
	mu_assert("POSITIVE != 1", pos == 1);

	return 0;
}

static char*
test_int_to_sign()
{
	sign neg = int_to_sign(-1);
	sign pos = int_to_sign(1);

	mu_assert("-1 != NEGATIVE", neg == NEGATIVE);
	mu_assert("+1 != POSITIVE", pos == POSITIVE);

	return 0;
}

static char*
test_arbint_eq()
{
	arbint a;
	arbint b;

	uint64_t test_array[3] = {1318934184, 121983, 0};
	a.value = test_array;
	a.length = 3;
	a.sign = POSITIVE;

	b.value = test_array;
	b.length = 3;
	b.sign = POSITIVE;

	mu_assert("Completely equal arbints aren't equal", arbint_eq(&a, &b) == true);


	uint64_t test_array_2[2] = {1318934184, 121983};
	a.value = test_array_2;
	a.length = 2;

	mu_assert("Numerically equal arbints with different length aren't equal",\
	          arbint_eq(&a, &b) == true);


	uint64_t zero_array[1] = {0};
	uint64_t one_array[1] = {1};

	a.value = zero_array;
	a.length = 1;
	a.sign = POSITIVE;

	b.value = zero_array;
	b.length = 1;
	b.sign = NEGATIVE;

	mu_assert("Arbints = 0 with different sign aren't equal", arbint_eq(&a, &b) == true);

	a.value = one_array;
	b.value = one_array;

	mu_assert("Arbints = 1 with different sign are equal", arbint_eq(&a, &b) == false);

	return 0;
}

static char*
test_print()
{
	printf("\n%s",
			"// This test would be a bit tricky to control automatically,\n"
			"// so it has to be checked visually right now. This bit\n"
			"// should be a valid C snippet declaring an arbint struct.\n");

	uint64_t value_array[3] = {1318934184, 121983, 0};
	arbint a = {
		.value = value_array,
		.length = 3,
		.sign = POSITIVE
	};

	print_arbint(&a);

	return 0;
}

static char*
all_tests()
{
	mu_run_test(test_char_to_digit);
	mu_run_test(test_sign_to_int);
	mu_run_test(test_int_to_sign);
	mu_run_test(test_print);
	mu_run_test(test_arbint_eq);
	return 0;
}

int
main(int argc, char **argv)
{
	char *result = all_tests();
	if (result != NULL)
	{
		printf("%s\n", result);
	}
	else
	{
		printf("ALL TESTS PASSED :D\n");
	}
	printf("Tests run: %d\n", tests_run);

	return result != 0;
}
