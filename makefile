
source_dir = src
include_dir = include
object_dir = obj
test_dir = test


test_executable = $(test_dir)/run-tests

CC = gcc
CFLAGS = --std=c99 --pedantic -I $(include_dir)

# List all .c files in the source directory
SRCS = $(wildcard $(source_dir)/*.c)

BASICS = $(include_dir)/datatypes.h

# For each one, there should be a corresponding .o file in the object directory
SRCS_basename = $(SRCS:src/%=%) # Remove the 'src/' (targets are named %.o, not obj/%.o)
OBJS = $(SRCS_basename:%.c=%.o) # Replace .c by .o

HEADERS = $(wildcard $(include_dir)/*.h)

# Print a variable by runnig 'make print-varname'
print-%:
	@echo $*=$($*)

default: test

run-test: test
	$(test_executable)

# Link all .o files together with test.o to make a test executable
test: test.o $(OBJS)
	$(CC) $(CFLAGS) -o $(test_executable) $(object_dir)/$< $(OBJS:%=$(object_dir)/%)

# Put an object file of test/test.c in obj/test.o
test.o: $(test_dir)/test.c $(test_dir)/minunit.h $(HEADERS)
	$(CC) $(CFLAGS) -I $(test_dir) -c $< -o $(object_dir)/$@

# All obj files except test.o are formed from from the corresponding .c and .h file.
# To be sure we have all headers as prerequisites, which isn't really necessary but
# actually detecting the minimal necessary prerequisites would be more hassle.
%.o: $(source_dir)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $(object_dir)/$@


.PHONY: clean
clean:
	rm -f *.o
	rm -f $(object_dir)/*.o
	rm -f $(include_dir)/*.h.gch
	rm -f $(test_executable)
