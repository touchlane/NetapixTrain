-include makefile

# Source, Executable, Library Defines.
LIB_SRC_DIR	 = src
LIB_TEST_DIR = test/src
LIB_SRC     := $(subst $(LIB_SRC_DIR)/,, $(shell find $(LIB_SRC_DIR) -maxdepth 1 -name '*.c')) 
TEST_SRC    := $(subst $(LIB_TEST_DIR)/,, $(shell find $(LIB_TEST_DIR) -name '*.c')) 
TEST_EXEC    = test/test
             
# Compiler, Include, Linker Defines.
CC           = gcc
LIB_INCLUDE  = -I./include/ -I./src/
LIB_CFTEST   = $(LIB_INCLUDE) -coverage -w -O0 -std=c99 -o $(TEST_EXEC)

all: lib_test

# Create a test running Executable with coverage turned on.
lib_test:
	$(CC) $(LIB_CFTEST) $(addprefix $(LIB_TEST_DIR)/, $(TEST_SRC)) $(addprefix $(LIB_SRC_DIR)/, $(LIB_SRC))
	rm -rf $(TEST_SRC:.c=.gcda) $(TEST_SRC:.c=.gcno)
	./test/test

# Clean Up Exectuable, Objects, Library, Coverage files
lib_test_clean:
	rm -rf $(TEST_EXEC)
	rm -rf $(TEST_SRC:.c=.gcda) $(TEST_SRC:.c=.gcno)

.PHONY: lib_test lib_test_clean
