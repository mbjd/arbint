source_dir := src
include_dir := include
object_dir := obj
test_dir := test

so_name := libarbint.so
test_executable := run-tests

lib_path := /usr/local/lib
inc_path := /usr/local/include

CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -pedantic -fpic -I $(include_dir)

# List all .c files in the source directory
SRCS := $(wildcard $(source_dir)/*.c)

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
test: $(object_dir)/test.o $(so_name)
	$(CC) $(CFLAGS) -L. -larbint -o $(test_executable) $<

# Put an object file of test/test.c in obj/test.o
$(object_dir)/test.o: $(test_dir)/test.c $(test_dir)/minunit.h $(HEADERS)
	$(CC) $(CFLAGS) -I $(test_dir) -c $< -o $@

# All obj files except test.o are formed from from the corresponding .c and .h file.
# To be sure we have all headers as prerequisites, which isn't really necessary but
# actually detecting the minimal necessary prerequisites would be more hassle.
.PHONY: $(object_dir)/%.o
$(object_dir)/%.o: $(source_dir)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Shared object
$(so_name): $(OBJS)
	$(CC) -shared -o $(so_name) $(OBJS)

.PHONY: install
install: $(so_name)
	cp $(so_name) $(lib_path)/$(so_name)
	cp $(include_dir)/arbint.h $(inc_path)/arbint.h

.PHONY: uninstall
uninstall: $(so_name)
	rm -f $(lib_path)/$(so_name)
	rm -f $(inc_path)/arbint.h

# Format all .c and .h files
.PHONY: pretty
pretty:
	@find . -name '*.c' -exec clang-format -i {} \;
	@find . -name '*.h' -exec clang-format -i {} \;
	@echo Formatted all .c and .h files with clang-format

# Calculate sha256 of all source code files
.PHONY: hash
hash:
	@find . -name '*.c' -print0 -o -name '*.h' -print0 | xargs -0 cat | gsha256sum | tr -d ' -'

# Count non-empty lines (code + comments) without unit testing
.PHONY: count
count:
	@{ find src -type f -print0; find include -type f -print0; } | xargs -0 cat | awk 'length>0 {print}' | wc -l | xargs

# Count non-empty lines in all .c and .h files
.PHONY: count-all
count-all:
	@{ find . -name '*.c' -print0 -o -name '*.h' -print0; } | xargs -0 cat | awk 'length>0 {print}' | wc -l | xargs

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(object_dir)/*.o
	rm -f $(include_dir)/*.h.gch
	rm -f $(test_executable)
	rm -f $(so_name)
