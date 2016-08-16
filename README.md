arbint
======

A small C library for arbitrary sized integers.

Right now it's very much an experimental piece of software. The reason it's so
small is that it still lacks a lot of basic features.

Numbers are represented in base 2^32 in an array of 32-bit unsigned integers. It's dynamically reallocated to fit larger numbers when necessary. The length of this array is stored separately, so that you can access the most significant digits without traversing the array à la `strlen`. This allows for efficient implementations of functions that don't require precision to the last digit, like logarithms.

Feel free to tinker around with it! To see the tests run, run `make` without arguments.

I'm happy to hear about suggestions, issues and ideas.

## Feature list

 - Parse a string containing a decimal number, and convert it to an arbint
 - Multiply an arbint by a 32-bit integer
 - Add together two arbints (if their sign is positive)
 - Add a 32-bit unsigned int raised to an integer power of 2^(32) to an arbint
 - Test two arbint's for numerical equality


## Todo list

- Remove confusion between `arbint` and `arbint*`
	- Have a typedef to define the basic type as a pointer to the actual
	  struct
- Adding a negative number should result in subtraction
- Put all 'public' functions and typedefs in a single header
- Fix segfault in `arbint_eq` when one of the structs is uninitialised
	- So structs are always 'initialised', but may contain random
	  gibberish. So my solution is to always call arbint_init on each
	  arbint right after creation.
- Construct `arbint` structs from strings or integers
	- From strings: done with `str_to_arbint`, from ints is the easy part
- Implement basic math functions with `arbint` structs and 'normal' numbers
	- Done: Add u32 to arbint, add arbint to arbint, multiply arbint by u32
- Return error codes if something goes wrong instead of printing a message
  to stderr and exiting


