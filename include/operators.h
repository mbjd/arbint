#ifndef OPERATORS_H
#define OPERATORS_H

#include <stddef.h>
#include <stdbool.h>

#include "datatypes.h"

// Check two arbints for numerical equality
// Returns false if different, true if equal
bool arbint_eq(arbint* a, arbint* b);

#endif
