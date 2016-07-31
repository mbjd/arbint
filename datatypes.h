#ifndef TYPE_H
#define TYPE_H

#include <stddef.h>

typedef enum sign { NEGATIVE, ZERO, POSITIVE } sign;

/*
 * Basic arbitrary-sized integer data type
 *
 * value:  Array of 64-bit unsigned ints that represents the integer value.
 *         The least significant number is at index 0 and stands for 1's.
 *         The next number, at index 1, counts in units of 2^64. The value
 *         can be interpreted as a base 2^64 number where each uint64_t is
 *         a 'digit', or as a continuous, long binary number.
 *
 * sign:   An enum storing the sign that can assume the values NEGATIVE,
 *         ZERO or POSITIVE.
 *
 * length: An integer storing the number of uint64_t's contained in value.
 *         Whenever value is reallocated to make more space, this value must
 *         be updated with the new size.
 */
typedef struct {
	uint64_t* value;
	enum sign sign; // +1, 0, or -1
	size_t length; // Number of uint64_t's in value
} arbint;

#endif
