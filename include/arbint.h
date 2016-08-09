#pragma once

#include <stdint.h>

#include "datatypes.h"
#include "helper-functions.h"
#include "operators.h"

arbint* str_to_arbint(char* input_str, arbint* to_fill, uint32_t base);

void arbint_init(arbint* new_arbint);
