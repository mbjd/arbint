#pragma once

#include <stdbool.h>

#include "datatypes.h"

int sign_to_int(sign enum_sign);

enum sign int_to_sign(int int_sign);

int char_to_digit(char c, uint32_t base);

void arbint_set_zero(arbint* to_reset);

bool is_digit(const char c);

bool addition_will_wrap(uint32_t a, uint32_t b);

void print_arbint(arbint* to_print);
