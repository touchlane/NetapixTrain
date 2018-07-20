-include example/config.mk

# Source, Executable, Library Defines.
APP_TEST_DIR = example/test/src
APP_SRC_DIR	 = example/src
TEST_SRC    := $(subst $(APP_TEST_DIR)/,, $(shell find $(APP_TEST_DIR) -name '*.c')) 
TEST_EXEC    = example/test/test

# Compiler, Include, Linker Defines.
CC           = gcc
APP_INCLUDE  = -I./include/ -I./$(APP_SRC_DIR)
APP_CFTEST   = $(APP_INCLUDE) -coverage -w -O0 -std=c99 -o $(TEST_EXEC)

all: example_test

# Create a test running Executable with coverage turned on.
example_test:
	$(CC) $(APP_CFTEST) $(addprefix $(APP_TEST_DIR)/, $(TEST_SRC)) $(addprefix $(APP_SRC_DIR)/, $(filter-out netapix.c, $(APP_SRC)))
	rm -rf $(TEST_SRC:.c=.gcda) $(TEST_SRC:.c=.gcno)
	example/test/test

# Clean Up Exectuable, Objects, Library, Coverage files d
example_test_clean:
	rm -rf $(TEST_EXEC)
	rm -rf $(TEST_SRC:.c=.gcda) $(TEST_SRC:.c=.gcno)

.PHONY: example_test example_test_clean