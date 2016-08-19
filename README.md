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
 - Subtract two arbints, regardless of sign and magnitude
 - Test two arbint's for numerical equality


## Todo list

- Have two sets of functions: One that is easy to use, that takes two
  values and returns a pointer to the result, and another one that only
  does the math and places the result in an already allocated buffer. This
  would completely separate the actual math from memory management, and it
  would encourage calculating the needed memory before the calculation
  instead of reallocating during the calculation. This would also be
  beneficial for running calculations in a loop, because you could allocate
  the space before the loop and clean it up afterwards, instead of having the
  math functions do that at every loop iteration.
- Adding a negative number should result in subtraction
- Put all 'public' functions and typedefs in a single header
- Return error codes if something goes wrong instead of printing a message
  to stderr and exiting


