arbint
======

A small C library for arbitrary sized integers.

Right now it's very much an experimental piece of software. The reason it's so
small is that it still lacks basic features like converting between `arbint`
structs and string representations, adding and subtracting `arbint`s, or doing
any kind of math with them.

What it can do is check for equality of two `arbint` structs, and run a bunch of
tests successfully :D

### Todo list:

- Fix segfault in `arbint_eq` when one of the structs is uninitialised
- Construct `arbint` structs from strings or integers
- Implement basic math functions with `arbint` structs and 'normal' numbers
- Return error codes if something goes wrong instead of printing a message to
  stderr and exiting


Feel free to tinker around with it! To see the tests run:

	make run-tests

I'm happy to hear about suggestions, issues and ideas.
