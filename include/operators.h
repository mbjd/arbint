#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "datatypes.h"

// Check two arbints for numerical equality
// Returns false if different, true if equal
bool arbint_eq(arbint a, arbint b);

void arbint_neg(arbint to_negate);

arbint arbint_add_positive(arbint a, arbint b);

arbint arbint_sub(arbint a, arbint b);

// TODO don't export this after debugging
arbint arbint_sub_primitive(arbint a, arbint b);

void add_to_arbint(arbint to_add, uint32_t value, size_t position);

// Returns true if a == 0, false if not
bool arbint_is_zero(arbint a);
