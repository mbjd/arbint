#pragma once

#include <stdint.h>

#include "datatypes.h"
#include "helper-functions.h"
#include "operators.h"

void str_to_arbint(char* input_str, arbint* to_fill, uint32_t base);

// Deallocates an arbint struct and its value.
void arbint_free(arbint* to_free);

// Deallocates the value only, for when the struct was statically allocated.
void arbint_free_static(arbint* to_free);
void arbint_init(arbint* new_arbint);

void arbint_mul(arbint* to_mul, uint32_t multiplier);

void arbint_to_str(arbint* to_convert, char** to_fill /*, uint32_t base*/);
