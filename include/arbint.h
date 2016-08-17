#pragma once

#include <stdint.h>

#include "datatypes.h"
#include "helper-functions.h"
#include "operators.h"

/* Constructor functions */

// Parse `input_str` in a given `base` and assign the numerical value to `to_fill`
//  - `base` can be between 2 and 36, inclusive
//  - `input_str` can include a leading +/- sign
//  - After that sign, `input_str` can only contain characters valid in the given `base`
//    '0'..'9' are interpreted as 0..9, 'A'..'Z' and 'a'..'z' as 10..35
//  - to_fill->value is (re)allocated when it's not large enough or when it's NULL
void str_to_arbint(char* input_str, arbint to_fill, uint32_t base);

// Construct an arbint from an uint64_t
//  - If the upper 32 bits are 0, the result will have length 1, otherwise 2
//  - The sign will always be positive
void u64_to_arbint(uint64_t value, arbint to_fill);


/* Initialisation functions */

// Allocate an arbint
//  - Returns an arbint with value=0, length=1, sign=POSITIVE
arbint arbint_new(void);

// Allocate an arbint
//  - Returns an arbint with value=0, length=`length`, sign=POSITIVE
//    -> Useful if you know that it will hold a large value and want to avoid realloc's
arbint arbint_new_length(size_t length);

// Allocate an arbint
//  - Returns an arbint filled with zero bytes
//    -> Its value is a NULL pointer and needs to be allocated manually before use!
arbint arbint_new_empty();


/* Reset functions */

// Resets an arbint to 0
//  - Reallocate the value array to contain one digit, set length to 1
//  - Set that digit to 0
//  - Set the sign to POSITIVE
void arbint_reset(arbint to_reset);

// Reset an arbint to 0
//  - Sets all digits to 0, but keeps length and sign
void arbint_set_zero(arbint new_arbint);

// Deallocate an arbint and its value.
void arbint_free(arbint to_free);

// Deallocate the value of `to_free` and set its value to the null pointer.
void arbint_free_value(arbint to_free);


/* Operator functions */

// Multiplies an arbint by a 32-bit unsigned int
//  - `to_mul` is reallocated if the result wouldn't fit otherwise
void arbint_mul(arbint to_mul, uint32_t multiplier);

// Should at some point convert to an actual string in arbitrary base
void arbint_to_str(arbint to_convert, char** to_fill /*, uint32_t base*/);

// Allocates and fills *to_fill with a hex representation of to_convert
void arbint_to_hex(arbint to_convert, char** to_fill);
