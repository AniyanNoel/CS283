# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target executable names
TARGET = dsh
TEST_TARGET = test_dshlib

# Find all source and header files
SRCS = $(wildcard *.c)
HDRS = $(wildcard *.h)

# Default target
all: $(TARGET) $(TEST_TARGET)

# Compile source to executable
$(TARGET): dsh_clc.c dshlib.c dshlib.h
	$(CC) $(CFLAGS) -o $(TARGET) dsh_clc.c dshlib.c

$(TEST_TARGET): test_dshlib.c dshlib.c dshlib.h
	$(CC) $(CFLAGS) -o $(TEST_TARGET) test_dshlib.c dshlib.c

# Clean up build files
clean:
	rm -f $(TARGET) $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)
	bats $(wildcard ./bats/*.sh)

valgrind:
	echo "pwd\nexit" | valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET) 
	echo "pwd\nexit" | valgrind --tool=helgrind --error-exitcode=1 ./$(TARGET) 

# Phony targets
.PHONY: all clean test