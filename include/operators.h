#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "datatypes.h"

// Check two arbints for numerical equality
// Returns false if different, true if equal
bool arbint_eq(arbint a, arbint b);

// Compares the value of two arbints
// a > b -> +1
// a == b -> 0
// a < b -> -1
int arbint_cmp(arbint a, arbint b);

// Other comparison functions
bool arbint_lt(arbint a, arbint b);  // a less than b
bool arbint_gt(arbint a, arbint b);  // a greater than b
bool arbint_leq(arbint a, arbint b); // a less than or equal b
bool arbint_geq(arbint a, arbint b); // a greater than or equal b

// Reverses the sign of `to_negate`.
void arbint_neg(arbint to_negate);

// Adds two arbints together, assuming they're both positive.
arbint arbint_add_primitive(arbint a, arbint b);

// Subtracts two arbints and returns the result in a newly allocated arbint
arbint arbint_sub(arbint a, arbint b);

void add_to_arbint(arbint to_add, uint32_t value, size_t position);

// Returns true if a == 0, false if not
bool arbint_is_zero(arbint a);
