#include <stdio.h>

#include "arbint.h"
#include "minunit.h"

int tests_run = 0;

static char* test_char_to_digit()
{
	mu_assert("char_to_digit('0') == 0", char_to_digit('0') == 0);
	mu_assert("char_to_digit('9') == 9", char_to_digit('9') == 9);
	mu_assert("char_to_digit('*') == -1", char_to_digit('*') == -1);
	mu_assert("char_to_digit('a') == -1", char_to_digit('a') == -1);
	mu_assert("char_to_digit('\\0') == -1", char_to_digit('\0') == -1);
	return 0;
}


static char* test_sign_to_int()
{
	int neg = sign_to_int(NEGATIVE);
	int zero = sign_to_int(ZERO);
	int pos = sign_to_int(POSITIVE);

	mu_assert("NEGATIVE != -1", neg == -1);
	mu_assert("ZERO != 0", zero == 0);
	mu_assert("POSITIVE != 1", pos == 1);

	return 0;
}

static char* test_int_to_sign()
{
	sign neg = int_to_sign(-1);
	sign zero = int_to_sign(0);
	sign pos = int_to_sign(1);

	mu_assert("-1 != NEGATIVE", neg == NEGATIVE);
	mu_assert("0 != ZERO", zero == ZERO);
	mu_assert("+1 != POSITIVE", pos == POSITIVE);

	return 0;
}

static char* all_tests()
{
	mu_run_test(test_char_to_digit);
	mu_run_test(test_sign_to_int);
	mu_run_test(test_int_to_sign);
	return 0;
}

int main(int argc, char **argv) {

	char *result = all_tests();
	if (result != 0)
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
