#ifndef HELPER_H
#define HELPER_H

#include "datatypes.h"

int sign_to_int(sign enum_sign);

enum sign int_to_sign(int int_sign);

int char_to_digit(char c);

bool is_digit(const char c);

bool addition_will_wrap(uint64_t a, uint64_t b);

void print_arbint(arbint* to_print);

#endif
