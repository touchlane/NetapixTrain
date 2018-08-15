-include makefile

# Source, Executable, Library Defines.
APP_SRC_DIR	 = example/src
APP_OBJ_DIR  = example/obj
APP_SRC     := $(subst $(APP_SRC_DIR)/,, $(shell find $(APP_SRC_DIR) -maxdepth 1 -name '*.c'))
APP_OBJ      = $(APP_SRC:.c=.o)
EXEC_PATH	 = example/bin
EXEC         = $(EXEC_PATH)/netapix

# Compiler, Include, Linker Defines.
CC           = gcc
APP_INCLUDE  = -I./include/ -I.$(APP_SRC_DIR)
APP_CFLAGS   = $(APP_INCLUDE) -w
LIBPATH      = -L./lib -lnetapix
LFLAGS       = -o $(EXEC) $(LIBPATH)

example_app: example_mkdir $(EXEC)

# Compile and Assemble C Source Files into Object Files.
$(APP_OBJ_DIR)/%.o: $(APP_SRC_DIR)/%.c
	$(CC) $(APP_CFLAGS) -c $< -o $@

# Compile binary and link with external Libraries.
$(EXEC): $(addprefix $(APP_OBJ_DIR)/, $(APP_OBJ))
	$(CC) $(APP_CFLAGS) $(LFLAGS) $(addprefix $(APP_OBJ_DIR)/, $(APP_OBJ))

# Create obj directory for .o files.
example_mkdir:
	mkdir -p $(APP_OBJ_DIR)
	mkdir -p $(EXEC_PATH)

# Clean Up Exectuable, Objects, Library, Coverage files d
example_clean:
	rm -rf $(EXEC_PATH) $(APP_OBJ_DIR)
	rm -rf $(APP_SRC:.c=.gcda) $(APP_SRC:.c=.gcno) 

.PHONY: example_all example_clean
