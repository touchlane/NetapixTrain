-include makefile

# Source, Executable, Library Defines.
LIB_SRC_DIR	 = src
LIB_TEST_DIR = test/src
LIB_SRC     := $(subst $(LIB_SRC_DIR)/,, $(shell find $(LIB_SRC_DIR) -maxdepth 1 -name '*.c')) 
TEST_SRC    := $(subst $(LIB_TEST_DIR)/,, $(shell find $(LIB_TEST_DIR) -name '*.c')) 
TEST_EXEC_PATH = test/bin
TEST_EXEC    = $(TEST_EXEC_PATH)/test
             
# Compiler, Include, Linker Defines.
CC           = gcc
LIB_INCLUDE  = -I./include/ -I./src/
LIB_CFTEST   = $(LIB_INCLUDE) -coverage -w -O0 -std=c99 -o $(TEST_EXEC)

# Create a test running Executable with coverage turned on.
lib_test: lib_test_mkdir
	$(CC) $(LIB_CFTEST) $(addprefix $(LIB_TEST_DIR)/, $(TEST_SRC)) $(addprefix $(LIB_SRC_DIR)/, $(LIB_SRC))
	rm -rf $(TEST_SRC:.c=.gcda) $(TEST_SRC:.c=.gcno)
	./test/bin/test

# Create obj directory for bin file.
lib_test_mkdir:
	mkdir -p $(TEST_EXEC_PATH)

# Clean Up Exectuable, Objects, Library, Coverage files
lib_test_clean:
	rm -rf $(TEST_EXEC_PATH)
	rm -rf $(TEST_SRC:.c=.gcda) $(TEST_SRC:.c=.gcno)

.PHONY: lib_test lib_test_clean
