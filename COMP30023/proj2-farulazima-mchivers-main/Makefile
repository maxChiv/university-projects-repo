TARGET = htproxy
CC = gcc
CFLAGS = -O3 -Wall -Wextra -g -I$(INC_DIR) # Add -I flag for include directory
GDBFLAG = -g -O0 # Used if needing to get extra compilation data for debugging

# Project directories
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include  # Directory where header files are located

# Find all source files in src and subdirectories
# Have to add a subdirectory compilation here if needed.
SRC = $(wildcard $(SRC_DIR)/*.c) \
      $(wildcard $(SRC_DIR)/util/*.c) \
      $(wildcard $(SRC_DIR)/linked-list/*.c)

# Generate corresponding object files
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Default target
all: $(TARGET)

# Link the executable
# 	ADD GDB FLAGS HERE IF NEEDED
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^  

# Compile each .c file into its corresponding .o
# 	ADD GDB FLAGS HERE IF NEEDED
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)	
	$(CC) $(CFLAGS) -c $< -o $@


# Format code
format:
	clang-format -style=file -i *.c

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGET)