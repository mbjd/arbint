#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "arbint.h"

int tests_run = 0;

static char*
test_char_to_digit()
{
	mu_assert("char_to_digit('0', 10) != 0", char_to_digit('0', 10) == 0);
	mu_assert("char_to_digit('9', 10) != 9", char_to_digit('9', 10) == 9);
	mu_assert("char_to_digit('*', 10) != -1", char_to_digit('*', 10) == -1);
	mu_assert("char_to_digit('a', 10) != -1", char_to_digit('a', 10) == -1);
	mu_assert("char_to_digit('\\0', 10) != -1", char_to_digit('\0', 10) == -1);
	mu_assert("char_to_digit('a', 11) != 10", char_to_digit('a', 11) == 10);
	mu_assert("char_to_digit('z', 36) != 35", char_to_digit('z', 36) == 35);
	mu_assert("char_to_digit('Z', 36) != 35", char_to_digit('Z', 36) == 35);

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
	uint32_t test_array_a[3] = {1318934184, 121983, 0};
	uint32_t test_array_b[3] = {1318934184, 121983, 0};
	uint32_t test_array_c[3] = {1318934185, 121983, 0}; // 1 more
	uint32_t test_array_d[2] = {1318934184, 121983};

	arbint a = {.value = test_array_a, .length = 3, .sign = POSITIVE};
	arbint b = {.value = test_array_b, .length = 3, .sign = POSITIVE};
	mu_assert("Completely equal arbints aren't equal",
	          arbint_eq(&a, &b) == true);

	a.value = test_array_c;
	mu_assert("Different arbints with same length are equal",
	          arbint_eq(&a, &b) == false);

	a.value  = test_array_d;
	a.length = 2;
	mu_assert("Numerically equal arbints with different length aren't equal",
	          arbint_eq(&a, &b) == true);

	uint32_t zero_array[1] = {0};
	uint32_t one_array[1]  = {1};

	a.value  = zero_array;
	a.length = 1;
	a.sign   = POSITIVE;

	b.value  = zero_array;
	b.length = 1;
	b.sign   = NEGATIVE;

	mu_assert("Arbints = 0 with different sign aren't equal",
	          arbint_eq(&a, &b) == true);

	a.value = one_array;
	b.value = one_array;

	mu_assert("Arbints = 1 with different sign are equal",
	          arbint_eq(&a, &b) == false);

	arbint c;
	arbint_init(&c);
	arbint d;
	arbint_init(&d);

	mu_assert("Empty arbints aren't equal", arbint_eq(&c, &d) == true);

	return 0;
}

// static char*
// test_str_to_arbint()
// {
// 	uint32_t* value_array = calloc(1, sizeof(uint32_t));
//
// 	arbint a = {
// 	    .value = value_array, .length = 1, .sign = POSITIVE,
// 	};
//
// 	printf("Now starting str_to_arbint test...\n");
// 	str_to_arbint("4294967296", &a, 10); // {0, 1, 0}
// 	print_arbint(&a);
//
// 	char* str;
// 	arbint_to_str(&a, &str, 10);
// 	printf("%s\n", str);
//
//
// 	// arbint_to_str(&a, &str, 10);
// 	// printf("%s\n", str);
//
// 	// todo assert
//
// 	return 0;
// }

static char*
test_arbint_mul()
{
	uint32_t* value_array_a = calloc(3, sizeof(uint32_t));
	value_array_a[0]        = 4294967295;
	value_array_a[1]        = 0;
	value_array_a[2]        = 0;
	arbint a = {.value = value_array_a, .length = 3, .sign = POSITIVE};
	arbint_mul(&a, 10);
	mu_assert("arbint_mul by 10 failed",
	          a.value[0] == 4294967286 && a.value[1] == 9 && a.value[2] == 0);

	uint32_t* value_array_b = calloc(3, sizeof(uint32_t));
	value_array_b[0]        = 4294967295;
	value_array_b[1]        = 4294967295;
	value_array_b[2]        = 0;
	arbint b = {.value = value_array_b, .length = 3, .sign = POSITIVE};
	arbint_mul(&b, 666);
	mu_assert("arbint_mul by 666 failed",
	          b.value[0] == 4294966630 && b.value[1] == 4294967295 &&
	              b.value[2] == 665);

	// This test should reallocate c.value to fit the larger value
	uint32_t* value_array_c = calloc(3, sizeof(uint32_t));
	value_array_c[0]        = 0;
	value_array_c[1]        = 0;
	value_array_c[2]        = 4200000000;
	arbint c = {.value = value_array_c, .length = 3, .sign = POSITIVE};
	print_arbint(&c);
	arbint_mul(&c, 1291);
	print_arbint(&c);
	mu_assert("arbint_mul didn't reallocate correctly", c.length == 4);
	mu_assert("arbint_mul by 1291 failed",
	          c.value[0] == 0 && c.value[1] == 0 && c.value[2] == 1951272448 &&
	              c.value[3] == 1262);

	return 0;
}

static char*
all_tests()
{
	mu_run_test(test_char_to_digit);
	mu_run_test(test_sign_to_int);
	mu_run_test(test_int_to_sign);
	mu_run_test(test_arbint_eq);
	mu_run_test(test_arbint_mul);
	// mu_run_test(test_str_to_arbint);
	return 0;
}

int
main()
{
	char* result = all_tests();
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
