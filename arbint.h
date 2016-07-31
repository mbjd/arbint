#ifndef ARBINT_H
#define ARBINT_H

#include <stdint.h>

#include "datatypes.h"
#include "helper-functions.h"

void add_binary_to_arbint(arbint bigint, int64_t value, int position);

int addition_will_wrap(uint64_t a, uint64_t b);

arbint* str_to_arbint(char* input_str);

#endif
