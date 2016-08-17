#pragma once

#include <stdint.h>

#include "datatypes.h"
#include "helper-functions.h"
#include "operators.h"

// Parsing functions
void str_to_arbint(char* input_str, arbint to_fill, uint32_t base);
void u64_to_arbint(uint64_t value, arbint to_fill);

// Initialise to +0
void arbint_init(arbint new_arbint);

// Allocate an arbint with value of length 1
arbint arbint_new(void);

// Allocate an arbint with value of length `length`
arbint arbint_new_length(size_t length);

// Allocate an arbint with a NULL pointer as value
arbint arbint_new_empty();

// Deallocates an arbint struct and its value.
void arbint_free(arbint to_free);

// Deallocates the value only, for when the struct was statically allocated.
void arbint_free_value(arbint to_free);

// Multiplies an arbint by a 32-bit unsigned int
void arbint_mul(arbint to_mul, uint32_t multiplier);

// Should at some point convert to an actual string in arbitrary base
void arbint_to_str(arbint to_convert, char** to_fill /*, uint32_t base*/);

// Allocates and fills *to_fill with a hex representation of to_convert
void arbint_to_hex(arbint to_convert, char** to_fill);
