source_dir = src
include_dir = include
object_dir = obj
test_dir = test

so_name = libarbint.so
test_executable = run-tests

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -I $(include_dir)

# Dependency tracking
# CFLAGS += -MMD
# -include *.d

# List all .c files in the source directory
SRCS := $(wildcard $(source_dir)/*.c)

BASICS = $(include_dir)/datatypes.h

# For each one, there should be a corresponding .o file in the object directory
SRCS_stripped := $(basename $(notdir $(SRCS))) # src/%.c -> %
OBJS := $(addsuffix .o,$(addprefix $(object_dir)/,$(SRCS_stripped)))

HEADERS := $(wildcard $(include_dir)/*.h)

# Print a variable by runnig 'make print-varname'
.PHONY: print-%
print-%:
	@echo $*=$($*)

default: run-tests

.PHONY: run-tests
run-tests: test
	./$(test_executable)

# Link all .o files together with test.o to make a test executable
.PHONY: test
test: $(object_dir)/test.o $(OBJS)
	$(CC) $(CFLAGS) -o $(test_executable) $< $(OBJS)

# Put an object file of test/test.c in obj/test.o
$(object_dir)/test.o: $(test_dir)/test.c $(test_dir)/minunit.h $(HEADERS)
	$(CC) $(CFLAGS) -I $(test_dir) -c $< -o $@

# All obj files except test.o are formed from from the corresponding .c and .h file.
# To be sure we have all headers as prerequisites, which isn't really necessary but
# actually detecting the minimal necessary prerequisites would be more hassle.
.PHONY: $(object_dir)/%.o
$(object_dir)/%.o: $(source_dir)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Format all .c and .h files
.PHONY: pretty
pretty:
	find . -name '*.c' -exec clang-format -i {} \;
	find . -name '*.h' -exec clang-format -i {} \;

$(so_name): $(OBJS)
	$(CC) -shared -o $(so_name) $(OBJS)

# Calculate sha256 of all source code files
.PHONY: hash
hash:
	find . -name '*.c' -print0 -o -name '*.h' -print0 | xargs -0 cat | gsha256sum

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(object_dir)/*.o
	rm -f $(include_dir)/*.h.gch
	rm -f $(test_executable)
	rm -f $(so_name)
