arbint
======

A small C library for arbitrary sized integers.

Right now it's very much an experimental piece of software. The reason it's so small is that it still lacks basic features like converting between  `arbint` structs and string representations, adding and subtracting `arbint`s, or doing any kind of math with them.

What it can do is check for equality of two `arbint` structs, and run a bunch of tests successfully :D

### Todo list:

- Construct `arbint` structs from strings or integers
- Implement basic math functions with `arbint` structs and 'normal' numbers
- Add some structure to the project

Feel free to tinker around with it! To see the tests run:

    make test
    ./run-tests

I'm happy to hear about suggestions, issues and ideas.
