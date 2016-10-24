#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "minunit.h"

#include "arbint.h"

int tests_run      = 0;
int assertions_run = 0;

static char*
test_char_to_digit()
{
	mu_assert_nm(char_to_digit('0', 10) == 0);
	mu_assert_nm(char_to_digit('9', 10) == 9);
	mu_assert_nm(char_to_digit('*', 10) == -1);
	mu_assert_nm(char_to_digit('a', 10) == -1);
	mu_assert_nm(char_to_digit('\0', 10) == -1);
	mu_assert_nm(char_to_digit('a', 11) == 10);
	mu_assert_nm(char_to_digit('z', 36) == 35);
	mu_assert_nm(char_to_digit('Z', 36) == 35);
	return 0;
}

static char*
test_sign_to_int()
{
	mu_assert_nm(sign_to_int(NEGATIVE) == -1);
	mu_assert_nm(sign_to_int(POSITIVE) == +1);
	return 0;
}

static char*
test_int_to_sign()
{
	mu_assert_nm(int_to_sign(+1) == POSITIVE);
	mu_assert_nm(int_to_sign(-1) == NEGATIVE);
	return 0;
}

static char*
test_arbint_eq()
{
	uint32_t test_array_a[3] = {1318934184, 121983, 0};
	uint32_t test_array_b[3] = {1318934184, 121983, 0};
	uint32_t test_array_c[3] = {1318934185, 121983, 0}; // 1 more
	uint32_t test_array_d[2] = {1318934184, 121983};

	arbint a = arbint_new_empty();
	arbint b = arbint_new_empty();

	a->value  = test_array_a;
	a->length = 3;
	a->sign   = POSITIVE;

	b->value  = test_array_b;
	b->length = 3;
	b->sign   = POSITIVE;

	mu_assert("Completely equal arbints aren't equal", arbint_eq(a, b) == true);

	a->value = test_array_c;
	mu_assert("Different arbints with same length are equal", arbint_eq(a, b) == false);

	a->value  = test_array_d;
	a->length = 2;
	mu_assert("Numerically equal arbints with different length aren't equal",
	          arbint_eq(a, b) == true);

	uint32_t zero_array[1] = {0};
	uint32_t one_array[1]  = {1};

	a->value  = zero_array;
	a->length = 1;
	a->sign   = POSITIVE;

	b->value  = zero_array;
	b->length = 1;
	b->sign   = NEGATIVE;

	mu_assert("Arbints = 0 with different sign aren't equal", arbint_eq(a, b) == true);

	a->value = one_array;
	b->value = one_array;

	mu_assert("Arbints = 1 with different sign are equal", arbint_eq(a, b) == false);

	// Don't use arbint_free because the value array is statically allocated
	free(a);
	free(b);

	arbint c = arbint_new();
	arbint d = arbint_new();

	mu_assert("Empty arbints aren't equal", arbint_eq(c, d) == true);

	arbint_free(c);
	arbint_free(d);

	return 0;
}

static char*
test_comparison()
{
	// arbint_cmp(a, b): a > b -> +1, a == b -> 0, a < b -> -1
	arbint a = arbint_new();
	arbint b = arbint_new();

	b->sign = NEGATIVE;
	mu_assert("arbint_cmp(+0, -0) != 0", arbint_cmp(a, b) == 0);

	str_to_arbint("5000", a, 10);
	str_to_arbint("5001", b, 10);
	mu_assert("arbint_cmp(5000, 5001) != -1", arbint_cmp(a, b) == -1);
	mu_assert("arbint_cmp(5001, 5000) != +1", arbint_cmp(b, a) == +1);

	arbint_neg(a);
	arbint_neg(b);
	mu_assert("arbint_cmp(-5000, -5001) != +1", arbint_cmp(a, b) == +1);
	mu_assert("arbint_cmp(-5001, -5000) != -1", arbint_cmp(b, a) == -1);
	arbint_neg(a);
	arbint_neg(b);

	arbint_reset(b);
	mu_assert("arbint_cmp(0, 5000) != -1", arbint_cmp(b, a) == -1);
	mu_assert("arbint_cmp(5000, 0) != +1", arbint_cmp(a, b) == +1);

	arbint_reset(a);
	mu_assert("arbint_cmp(0, 0) != 0", arbint_cmp(a, b) == 0);

	str_to_arbint("99999999999999999999999", a, 10);
	mu_assert("arbint_cmp(999...999, 0) != +1", arbint_cmp(a, b) == +1);
	mu_assert("arbint_cmp(0, 999...999) != -1", arbint_cmp(b, a) == -1);

	str_to_arbint("-01413334130410576106307605713267149637", b, 10);
	mu_assert("arbint_cmp(999...999, -141...637) != +1", arbint_cmp(a, b) == +1);
	mu_assert("arbint_cmp(-141...637, 999...999) != -1", arbint_cmp(b, a) == -1);

	mu_assert("arbint_cmp(x, x) != 0", !arbint_cmp(a, a) && !arbint_cmp(b, b));

	arbint_free(a);
	arbint_free(b);
	return 0;
}

static char*
test_str_to_arbint()
{
	arbint a = arbint_new();
	str_to_arbint("4294967296", a, 10);
	mu_assert("str_to_arbint with UINT32_MAX failed",
	          a->value[0] == 0 && a->value[1] == 1 && a->length == 2 &&
	              a->sign == POSITIVE);

	arbint_free_value(a);
	arbint_reset(a);
	str_to_arbint("-4294967296", a, 10);
	mu_assert("str_to_arbint with -UINT32_MAX failed",
	          a->value[0] == 0 && a->value[1] == 1 && a->length == 2 &&
	              a->sign == NEGATIVE);

	arbint_free_value(a);
	arbint_reset(a);
	str_to_arbint("+792384103083241340432014773910347139419741", a, 10);
	mu_assert("str_to_arbint with random huge value and leading + failed",
	          a->value[0] == 313953885 && a->value[1] == 3019150336 &&
	              a->value[2] == 3284471345 && a->value[3] == 2609588367 &&
	              a->value[4] == 2328 && a->length == 5 && a->sign == POSITIVE);

	arbint_free_value(a);
	arbint_reset(a);
	str_to_arbint("-792384103083241340432014773910347139419741", a, 10);
	mu_assert("str_to_arbint with -(random huge value) failed",
	          a->value[0] == 313953885 && a->value[1] == 3019150336 &&
	              a->value[2] == 3284471345 && a->value[3] == 2609588367 &&
	              a->value[4] == 2328 && a->length == 5 && a->sign == NEGATIVE);

	arbint_free_value(a);
	arbint_reset(a);
	// 116149714575680328862165199336710216176981 in base 10
	str_to_arbint("-010101010101010101010101010101010101010101010101010101010101010101010"
	              "101010101010101010101010101010101010101010101010101010101010101010101",
	              a, 2);
	uint32_t x = 1431655765;
	mu_assert("str_to_arbint with base 2 failed",
	          a->length == 5 && a->value[0] == x && a->value[1] == x &&
	              a->value[2] == x && a->value[3] == x && a->value[4] == 341 &&
	              a->sign == NEGATIVE);

	arbint_free_value(a);
	arbint_reset(a);
	str_to_arbint("2234437233536512670610467177347354552643414515707450555642435166545370"
	              "737574172732620750174516443207",
	              a, 8);
	mu_assert("str_to_arbint with base 8 failed",
	          a->length == 10 && a->value[0] == 3845801607 && a->value[1] == 1991384707 &&
	              a->value[2] == 3820977213 && a->value[3] == 2745658155 &&
	              a->value[4] == 2401810152 && a->value[5] == 1513645367 &&
	              a->value[6] == 2138290989 && a->value[7] == 2915443310 &&
	              a->value[8] == 2410081236 && a->value[9] == 1180);

	arbint_free(a);

	return 0;
}

static char*
test_arbint_mul()
{
	arbint a    = arbint_new_length(3);
	a->value[0] = 4294967295;
	arbint_mul(a, 10);
	mu_assert("arbint_mul by 10 failed",
	          a->value[0] == 4294967286 && a->value[1] == 9 && a->value[2] == 0);
	arbint_free(a);

	arbint b    = arbint_new_length(3);
	b->value[0] = 4294967295;
	b->value[1] = 4294967295;
	arbint_mul(b, 666);
	mu_assert("arbint_mul by 666 failed", b->value[0] == 4294966630 &&
	                                          b->value[1] == 4294967295 &&
	                                          b->value[2] == 665);
	arbint_free(b);

	// This test should put the most significant digit close to overflowing,
	// but should not allocate new space in c.value.
	arbint c    = arbint_new_length(3);
	c->value[0] = 4294967295;
	c->value[1] = 4294967295;
	arbint_mul(c, UINT32_MAX);
	mu_assert("arbint_mul by UINT32_MAX reallocated unnecessarily", c->length == 3);
	mu_assert("arbint_mul by UINT32_MAX failed",
	          c->value[0] == 1 && c->value[1] == 4294967295 && c->value[2] == 4294967294);
	arbint_free(c);

	// This test should reallocate c.value to fit the larger value
	arbint d    = arbint_new_length(3);
	d->value[2] = 4200000000;
	arbint_mul(d, 1291);
	mu_assert("arbint_mul by 1291 didn't reallocate correctly", d->length == 4);
	mu_assert("arbint_mul by 1291 failed", d->value[0] == 0 && d->value[1] == 0 &&
	                                           d->value[2] == 1951272448 &&
	                                           d->value[3] == 1262);
	arbint_free(d);

	// Test multiplication by 1
	arbint e = arbint_new();
	arbint f = arbint_new();

	str_to_arbint("717171717171717171717", e, 10);
	str_to_arbint("717171717171717171717", f, 10);

	mu_assert("arbint_eq doesn't work in arbint_mul test", arbint_eq(e, f));

	arbint_mul(f, 1);

	mu_assert("arbint_mul by 1 doesn't return the same value", arbint_eq(e, f));


	return 0;
}

static char*
test_str_mul_eq()
{
	arbint b = arbint_new();
	arbint c = arbint_new();

	// Initialize so that c = 10 * b
	str_to_arbint("999999999999999999999999", b, 10);
	str_to_arbint("9999999999999999999999990", c, 10);
	// Multiply so that c = b
	arbint_mul(b, 10);
	mu_assert("either arbint_eq or arbint_mul doesn't work (1)", arbint_eq(b, c));

	arbint_reset(b);
	arbint_reset(c);
	// Initialize so that c = 10 * b
	str_to_arbint("-77777777777777777777777777777777777777", b, 10);
	str_to_arbint("-7777777777777777777777777777777777777700000000", c, 10);
	// Multiply so that c = b
	arbint_mul(b, 100000000);
	mu_assert("either arbint_eq or arbint_mul doesn't work (2)", arbint_eq(b, c));

	arbint_free(b);
	arbint_free(c);

	return 0;
}

static char*
test_u64_to_arbint()
{
	// Test uninitialised
	arbint a = arbint_new();
	u64_to_arbint(9120311729134, a);
	mu_assert("u64_to_arbint failed (1)", a->length == 2 && a->value[0] == 2096159726 &&
	                                          a->value[1] == 2123 && a->sign == POSITIVE);

	// Test initialised
	arbint b = arbint_new_empty();
	u64_to_arbint(9120311729134, b);
	mu_assert("u64_to_arbint failed (2)", b->length == 2 && b->value[0] == 2096159726 &&
	                                          b->value[1] == 2123 && b->sign == POSITIVE);

	// Test with 2^32 - 1
	arbint c = arbint_new();
	u64_to_arbint(UINT32_MAX, c);
	mu_assert("u64_to_arbint with UINT64_MAX alloc's too much", c->length == 1);
	mu_assert("u64_to_arbint with UINT64_MAX failed", c->value[0] == UINT32_MAX);

	// Test with 2^64 - 1
	arbint d = arbint_new();
	u64_to_arbint(UINT64_MAX, d);
	mu_assert("u64_to_arbint with UINT64_MAX failed",
	          d->length == 2 && d->value[0] == UINT32_MAX && d->value[1] == UINT32_MAX);

	arbint_free(a);
	arbint_free(b);
	arbint_free(c);
	arbint_free(d);

	return 0;
}

static char*
test_arbint_copy()
{
	arbint a = arbint_new();
	str_to_arbint("+792384103083241340432014773910347139419741", a, 10);
	mu_assert("str_to_arbint in test_arbint_copy failed",
	          a->value[0] == 313953885 && a->value[1] == 3019150336 &&
	              a->value[2] == 3284471345 && a->value[3] == 2609588367 &&
	              a->value[4] == 2328 && a->length == 5 && a->sign == POSITIVE);

	arbint b = arbint_copy(a);

	mu_assert("arbint_copy changed values",
	          b->value[0] == 313953885 && b->value[1] == 3019150336 &&
	              b->value[2] == 3284471345 && b->value[3] == 2609588367 &&
	              b->value[4] == 2328 && b->length == 5 && b->sign == POSITIVE);

	mu_assert("arbint_copy returned same pointer instead of copying",
	          b->value != a->value);

	return 0;
}

static char*
test_set_zero_and_reset()
{
	arbint a = arbint_new();
	str_to_arbint("7777777777777777777777", a, 10);
	mu_assert("str_to_arbint in test_set_zero_and_reset failed", a->length == 3);

	arbint_set_zero(a);
	mu_assert("arbint_set_zero did not set arbint to zero", arbint_is_zero(a));

	arbint_reset(a);
	mu_assert("arbint_reset did not set length to 1", a->length == 1);

	return 0;
}

static char*
test_arbint_add()
{
	arbint a = arbint_new();
	arbint b = arbint_new();
	arbint c = arbint_new();

	str_to_arbint("100", a, 10);
	str_to_arbint("200", b, 10);
	str_to_arbint("300", c, 10);

	arbint result = arbint_add(a, b);

	mu_assert("arbint_add doesn't work with short numbers", arbint_eq(c, result));
	arbint_free(result);

	str_to_arbint("222222222222222222222222222222222222", a, 10);
	str_to_arbint("444444444444444444444444444444444444", b, 10);
	str_to_arbint("666666666666666666666666666666666666", c, 10);
	result = arbint_add(a, b);
	mu_assert("arbint_add doesn't work with long numbers", arbint_eq(c, result));
	arbint_free(result);

	str_to_arbint("8888888888888888888888888888888888888", a, 10);
	str_to_arbint("-3333333333333333333333333333333333333", b, 10);
	str_to_arbint("5555555555555555555555555555555555555", c, 10);
	result = arbint_add(a, b);
	mu_assert("arbint_add: 8.. + (-3..) != 5..", arbint_eq(result, c));
	arbint_free(result);
	result = arbint_add(b, a);
	mu_assert("arbint_add: (-3..) + 8.. != 5..", arbint_eq(result, c));
	arbint_free(result);

	arbint_neg(a);
	arbint_neg(b);
	arbint_neg(c);

	result = arbint_add(a, b);
	mu_assert("arbint_add: (-8..) + 3.. != -5..", arbint_eq(result, c));
	arbint_free(result);
	result = arbint_add(b, a);
	mu_assert("arbint_add: 3.. + (-8..) != -5..", arbint_eq(result, c));
	arbint_free(result);

	b->sign = NEGATIVE;
	result  = arbint_add(b, c);
	mu_assert("arbint_add: (-3..) + (-5..) != -(8..)", arbint_eq(result, a));
	arbint_free(result);

	arbint_free(a);
	arbint_free(b);
	arbint_free(c);

	return 0;
}

static char*
test_arbint_sub()
{
	arbint a = arbint_new();
	arbint b = arbint_new();
	arbint c = arbint_new();

	arbint result;

	result = arbint_sub(a, b);
	mu_assert("arbint_sub: 0 - 0 != 0", arbint_is_zero(result));
	arbint_free(result);

	b->sign = NEGATIVE;
	result  = arbint_sub(a, b);
	mu_assert("arbint_sub: 0 - (-0) != 0", arbint_is_zero(result));
	arbint_free(result);

	result = arbint_sub(a, a);
	mu_assert("arbint_sub: a - a != 0", arbint_is_zero(result));
	arbint_free(result);

	str_to_arbint("777777777777777777777777", a, 10);
	str_to_arbint("333333333333333333333333", b, 10);
	str_to_arbint("444444444444444444444444", c, 10);

	result = arbint_sub(a, b);
	mu_assert("arbint_sub: 77..77 - 33..33 != 44..44", arbint_eq(c, result));
	arbint_free(result);

	arbint_neg(c);
	result = arbint_sub(b, a);
	mu_assert("arbint_sub: 33..33 - 77..77 != -44..44", arbint_eq(c, result));
	arbint_free(result);

	str_to_arbint("4000000000000000000000", a, 10);
	str_to_arbint("4000000000000000000000", b, 10);

	result = arbint_sub(a, b);
	mu_assert("arbint_sub: 40..00 - 40..00 != 0", arbint_is_zero(result));
	arbint_free(result);

	arbint_neg(b);
	result = arbint_sub(a, b);
	str_to_arbint("8000000000000000000000", c, 10);
	mu_assert("arbint_sub: 40..00 - (-40..00) != +80..00", arbint_eq(c, result));
	arbint_free(result);

	arbint_neg(c);
	result = arbint_sub(b, a);
	mu_assert("arbint_sub: -40..00 - 40..00 != -80..00", arbint_eq(c, result));
	arbint_free(result);

	arbint_neg(a);
	mu_assert("arbint_sub: test wrong", a->sign == NEGATIVE && b->sign == NEGATIVE);
	result = arbint_sub(a, b);
	mu_assert("arbint_sub: -40..00 - (-40..00) != 0", arbint_is_zero(result));
	arbint_free(result);

	str_to_arbint("-6000000000000000000000", a, 10);
	str_to_arbint("-4000000000000000000000", b, 10);
	str_to_arbint("-2000000000000000000000", c, 10);
	result = arbint_sub(a, b);
	mu_assert("arbint_sub: -60..00 - (-40..00) != -20..00", arbint_eq(result, c));
	arbint_free(result);

	arbint_neg(c);
	result = arbint_sub(b, a);
	mu_assert("arbint_sub: -40..00 - (-60..00) != 20..00", arbint_eq(result, c));
	arbint_free(result);

	arbint_free(a);
	arbint_free(b);
	arbint_free(c);
	return 0;
}

static char*
test_arbint_to_hex()
{
	arbint a = arbint_new();
	str_to_arbint("999999999999999999999999999999999999", a, 10);
	char** result = malloc(sizeof(char*));
	arbint_to_hex(a, result);
	mu_assert("arbint_to_hex failed (999...999)",
	          !strcmp(*result, "C097CE7BC90715B34B9F0FFFFFFFFF"));
	free(*result);

	arbint_reset(a);
	str_to_arbint("-999999999999999999999999999999999999", a, 10);
	arbint_to_hex(a, result);
	mu_assert("arbint_to_hex failed (-999...999)",
	          !strcmp(*result, "-C097CE7BC90715B34B9F0FFFFFFFFF"));
	free(*result);

	arbint_reset(a);
	str_to_arbint("DeaDBeEF", a, 16);
	arbint_to_hex(a, result);
	mu_assert("arbint_to_hex failed (0xDEADBEEF)", !strcmp(*result, "DEADBEEF"));
	free(*result);

	arbint_reset(a);
	str_to_arbint("-deadbeefDEADBEEFdeadbeefDEADBEEF", a, 16);
	arbint_to_hex(a, result);
	mu_assert("arbint_to_hex failed (0xdeadbeefDEADBEEF...)",
	          !strcmp(*result, "-DEADBEEFDEADBEEFDEADBEEFDEADBEEF"));
	free(*result);

	arbint_free(a);
	free(result);
	return 0;
}

static char*
test_highest_digit()
{
	arbint a    = arbint_new_length(8);
	a->length   = 8;
	a->sign     = POSITIVE;
	a->value[0] = 10;
	a->value[4] = 10;
	mu_assert("arbint_highest_digit failed (1)", arbint_highest_digit(a) == 4);

	str_to_arbint("-deadd00d121337", a, 16);
	mu_assert("arbint_highest_digit failed (2)", arbint_highest_digit(a) == 1);
	return 0;
}

static char*
test_trim()
{
	arbint a = arbint_new_empty();

	str_to_arbint("9999999999999999999999999999999999999", a, 10);
	mu_assert("str_to_arbint in test_trim failed", a->length == 4);

	str_to_arbint("4294967296", a, 10);
	mu_assert("str_to_arbint changed length", a->length == 4);

	arbint_trim(a);
	mu_assert("arbint_trim failed to reduce length", a->length == 2);

	arbint_free(a);

	return 0;
}

static char*
test_init_functions()
{
	arbint a = arbint_new();
	mu_assert("arbint_new did not produce length 1", a->length == 1);
	mu_assert("arbint_new did not produce value 0", arbint_is_zero(a));
	mu_assert("arbint_new did not produce sign POSITIVE", a->sign == POSITIVE);
	arbint_free(a);

	arbint b = arbint_new_length(5);
	mu_assert("arbint_new_length did not produce correct length", b->length == 5);
	mu_assert("arbint_new_length did not produce value 0", arbint_is_zero(b));
	mu_assert("arbint_new_length did not produce sign POSITIVE", b->sign == POSITIVE);
	arbint_free(b);

	arbint c = arbint_new_empty();
	mu_assert("arbint_new_empty did not produce length 0", c->length == 0);
	mu_assert("arbint_new_empty did not produce value NULL", c->value == NULL);
	arbint_free(c);

	return 0;
}

static char*
all_tests()
{
	// Basics
	mu_run_test(test_char_to_digit);
	mu_run_test(test_sign_to_int);
	mu_run_test(test_int_to_sign);
	mu_run_test(test_highest_digit);
	mu_run_test(test_trim);

	// Constructors/parsing
	mu_run_test(test_init_functions);
	mu_run_test(test_str_to_arbint);
	mu_run_test(test_u64_to_arbint);

	// Operators
	mu_run_test(test_arbint_eq);
	mu_run_test(test_comparison);
	mu_run_test(test_arbint_mul);
	mu_run_test(test_str_mul_eq);
	mu_run_test(test_arbint_add);
	mu_run_test(test_arbint_sub);

	// Memory management etc.
	mu_run_test(test_arbint_copy);
	mu_run_test(test_set_zero_and_reset);

	// Output
	mu_run_test(test_arbint_to_hex);
	return 0;
}

int
main()
{
	char* result = all_tests();
	if (result)
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
