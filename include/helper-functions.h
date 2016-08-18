#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "datatypes.h"

int sign_to_int(sign enum_sign);

enum sign int_to_sign(int int_sign);

int char_to_digit(char c, uint32_t base);

bool is_digit(const char c);

bool addition_will_wrap(uint32_t a, uint32_t b);

arbint arbint_copy(arbint src);

size_t arbint_highest_digit(arbint input);

void arbint_trim(arbint to_trim);

/* Print functions (for debugging) */

// Print the code that would reproduce the given arbint with str_to_arbint.
// For example, if we have:
// 	arbint a = arbint_new();
// 	str_to_arbint("-200", a, 10);
// calling print_arbint(a) would print:
// 	arbint a = arbint_new();
// 	str_to_arbint("-C8", a, 16);
void print_arbint(arbint to_print);

// Print the code that would reproduce the given arbint by assigning the
// values manually. For example, if we have:
// 	arbint a = arbint_new();
// 	str_to_arbint("-200", a, 10);
// calling print_arbint_verbose(a) would print:
// 	arbint a = arbint_new_length(1);
// 	a->value[0] = 200;
// 	a->sign = NEGATIVE;
void print_arbint_verbose(arbint to_print);
