#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"

#include "arbint.h"

int tests_run      = 0;
int assertions_run = 0;

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
	mu_assert("Completely equal arbints aren't equal", arbint_eq(&a, &b) == true);

	a.value = test_array_c;
	mu_assert("Different arbints with same length are equal", arbint_eq(&a, &b) == false);

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

	mu_assert("Arbints = 0 with different sign aren't equal", arbint_eq(&a, &b) == true);

	a.value = one_array;
	b.value = one_array;

	mu_assert("Arbints = 1 with different sign are equal", arbint_eq(&a, &b) == false);

	arbint c;
	arbint_init(&c);
	arbint d;
	arbint_init(&d);

	mu_assert("Empty arbints aren't equal", arbint_eq(&c, &d) == true);

	return 0;
}

static char*
test_str_to_arbint()
{
	arbint a;
	arbint_init(&a);
	str_to_arbint("4294967296", &a, 10);
	mu_assert("str_to_arbint with UINT32_MAX failed",
	          a.value[0] == 0 && a.value[1] == 1 && a.length == 2 && a.sign == POSITIVE);

	arbint_init(&a);
	str_to_arbint("-4294967296", &a, 10);
	mu_assert("str_to_arbint with -UINT32_MAX failed",
	          a.value[0] == 0 && a.value[1] == 1 && a.length == 2 && a.sign == NEGATIVE);

	arbint_init(&a);
	str_to_arbint("+792384103083241340432014773910347139419741", &a, 10);
	mu_assert("str_to_arbint with random huge value and leading + failed",
	          a.value[0] == 313953885 && a.value[1] == 3019150336 &&
	              a.value[2] == 3284471345 && a.value[3] == 2609588367 &&
	              a.value[4] == 2328 && a.length == 5 && a.sign == POSITIVE);

	str_to_arbint("-792384103083241340432014773910347139419741", &a, 10);
	mu_assert("str_to_arbint with -(random huge value) failed",
	          a.value[0] == 313953885 && a.value[1] == 3019150336 &&
	              a.value[2] == 3284471345 && a.value[3] == 2609588367 &&
	              a.value[4] == 2328 && a.length == 5 && a.sign == NEGATIVE);

	return 0;
}

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
	          b.value[0] == 4294966630 && b.value[1] == 4294967295 && b.value[2] == 665);

	// This test should put the most significant digit close to overflowing,
	// but should not allocate new space in c.value.
	uint32_t* value_array_c = calloc(3, sizeof(uint32_t));
	value_array_c[0]        = 4294967295;
	value_array_c[1]        = 4294967295;
	value_array_c[2]        = 0;
	arbint c = {.value = value_array_c, .length = 3, .sign = POSITIVE};
	arbint_mul(&c, UINT32_MAX);
	mu_assert("arbint_mul by UINT32_MAX reallocated unnecessarily", c.length == 3);
	mu_assert("arbint_mul by UINT32_MAX failed",
	          c.value[0] == 1 && c.value[1] == 4294967295 && c.value[2] == 4294967294);

	// This test should reallocate c.value to fit the larger value
	uint32_t* value_array_d = calloc(3, sizeof(uint32_t));
	value_array_d[0]        = 0;
	value_array_d[1]        = 0;
	value_array_d[2]        = 4200000000;
	arbint d = {.value = value_array_d, .length = 3, .sign = POSITIVE};
	arbint_mul(&d, 1291);
	mu_assert("arbint_mul by 1291 didn't reallocate correctly", d.length == 4);
	mu_assert("arbint_mul by 1291 failed",
	          d.value[0] == 0 && d.value[1] == 0 && d.value[2] == 1951272448 &&
	              d.value[3] == 1262);


	arbint_free_static(&a);
	arbint_free_static(&b);
	arbint_free_static(&c);
	arbint_free_static(&d);

	return 0;
}

static char*
test_str_mul_eq()
{
	arbint b;
	arbint c;

	arbint_init(&b);
	arbint_init(&c);
	// Initialize so that c = 10 * b
	str_to_arbint("999999999999999999999999", &b, 10);
	str_to_arbint("9999999999999999999999990", &c, 10);
	// Multiply so that c = b
	arbint_mul(&b, 10);
	mu_assert("either arbint_eq or arbint_mul doesn't work (1)", arbint_eq(&b, &c));

	arbint_init(&b);
	arbint_init(&c);
	// Initialize so that c = 10 * b
	str_to_arbint("-77777777777777777777777777777777777777", &b, 10);
	str_to_arbint("-7777777777777777777777777777777777777700000000", &c, 10);
	// Multiply so that c = b
	arbint_mul(&b, 100000000);
	mu_assert("either arbint_eq or arbint_mul doesn't work (2)", arbint_eq(&b, &c));

	arbint_free_static(&b);
	arbint_free_static(&c);

	return 0;
}

static char*
test_u64_to_arbint()
{
	// Test uninitialised
	arbint a;
	u64_to_arbint(9120311729134, &a);
	mu_assert("u64_to_arbint failed (1)",
	          a.length == 2 && a.value[0] == 2096159726 && a.value[1] == 2123 &&
	              a.sign == POSITIVE);

	// Test initialised
	arbint b;
	arbint_init(&b);
	u64_to_arbint(9120311729134, &b);
	mu_assert("u64_to_arbint failed (2)",
	          b.length == 2 && b.value[0] == 2096159726 && b.value[1] == 2123 &&
	              b.sign == POSITIVE);

	// Test with 2^32 - 1
	arbint c;
	u64_to_arbint(UINT32_MAX, &c);
	mu_assert("u64_to_arbint with UINT64_MAX alloc's too much", c.length == 1);
	mu_assert("u64_to_arbint with UINT64_MAX failed", c.value[0] == UINT32_MAX);

	// Test with 2^64 - 1
	arbint d;
	u64_to_arbint(UINT64_MAX, &d);
	mu_assert("u64_to_arbint with UINT64_MAX failed",
	          d.length == 2 && d.value[0] == UINT32_MAX && d.value[1] == UINT32_MAX);

	arbint_free_static(&a);
	arbint_free_static(&b);
	arbint_free_static(&c);
	arbint_free_static(&d);

	return 0;
}

static char*
test_arbint_copy()
{
	arbint a;
	arbint_init(&a);
	str_to_arbint("+792384103083241340432014773910347139419741", &a, 10);
	mu_assert("str_to_arbint in test_arbint_copy failed",
	          a.value[0] == 313953885 && a.value[1] == 3019150336 &&
	              a.value[2] == 3284471345 && a.value[3] == 2609588367 &&
	              a.value[4] == 2328 && a.length == 5 && a.sign == POSITIVE);

	arbint* b = arbint_copy(&a);

	mu_assert("arbint_copy changed values",
	          b->value[0] == 313953885 && b->value[1] == 3019150336 &&
	              b->value[2] == 3284471345 && b->value[3] == 2609588367 &&
	              b->value[4] == 2328 && b->length == 5 && b->sign == POSITIVE);

	mu_assert("arbint_copy returned same pointer instead of copying",
	          b->value != a.value);

	return 0;
}

static char*
test_arbint_add_positive()
{
	arbint a;
	arbint b;
	arbint expected;
	arbint_init(&a);
	arbint_init(&b);
	arbint_init(&expected);

	str_to_arbint("100", &a, 10);
	str_to_arbint("200", &b, 10);
	str_to_arbint("300", &expected, 10);

	arbint* result = arbint_add_positive(&a, &b);

	mu_assert("arbint_add doesn't work with short numbers", arbint_eq(&expected, result));
	arbint_free(result);

	str_to_arbint("222222222222222222222222222222222222", &a, 10);
	str_to_arbint("444444444444444444444444444444444444", &b, 10);
	str_to_arbint("666666666666666666666666666666666666", &expected, 10);

	result = arbint_add_positive(&a, &b);

	mu_assert("arbint_add doesn't work with long numbers", arbint_eq(&expected, result));

	arbint_free_static(&a);
	arbint_free_static(&b);
	arbint_free_static(&expected);
	arbint_free(result);

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
	mu_run_test(test_arbint_add_positive);
	mu_run_test(test_str_to_arbint);
	mu_run_test(test_str_mul_eq);

	mu_run_test(test_arbint_copy);

	mu_run_test(test_u64_to_arbint);
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
		printf("\nALL TESTS PASSED :D\n");
	}
	printf("%d tests, %d assertions\n", tests_run, assertions_run);

	return result != 0;
}
