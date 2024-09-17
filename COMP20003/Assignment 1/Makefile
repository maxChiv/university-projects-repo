# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Source files and target
DICT1_TARGET = dict1
DICT1_SRC = driver.c dict.c io.c linkedlist.c data.c
DICT1_OBJ = $(DICT1_SRC:.c=.o)
DICT2_TARGET = dict2
DICT2_SRC = driver.c dict.c io.c linkedlist.c data.c
DICT2_OBJ = $(DICT2_SRC:.c=.o)

# Stages for input
STAGE1 = 1
STAGE2 = 2

# Explicit dependencies
driver.o: dict.h linkedlist.h data.h
dict.o: linkedlist.h data.h io.h
io.o: linkedlist.h

# Rule to build the object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build dict1
$(DICT1_TARGET): $(DICT1_OBJ)
	$(CC) $(CFLAGS) $(DICT1_OBJ) -o $(DICT1_TARGET)

# Rule to build dict2
$(DICT2_TARGET): $(DICT2_OBJ)
	$(CC) $(CFLAGS) $(DICT2_OBJ) -o $(DICT2_TARGET)

# Rule to run dict1 with Valgrind
valgrind_dict1: $(DICT1_TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(DICT1_TARGET) $(STAGE1) tests/dataset_full.csv my_outputs/testfull.out < tests/testfull.in > my_outputs/testfull.stdout.out

# Rule to run dict2 with Valgrind
valgrind_dict2: $(DICT2_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(DICT2_TARGET) $(STAGE2) tests/dataset_del.csv my_outputs/delfull.out < tests/del3.in > my_outputs/delfull.stdout.out

# Rule to run dict1 with GDB
gdb_dict1: $(DICT1_TARGET)
	gdb ./$(DICT1_TARGET)

# Rule to run dict2 with GDB
gdb_dict2: $(DICT2_TARGET)
	gdb ./$(DICT2_TARGET)


# Clean up build artifacts
clean:
	rm -f $(DICT1_OBJ) $(DICT2_OBJ) $(DICT1_TARGET) $(DICT2_TARGET)

# Testing ----/
# example
# ./$(DICT?_TARGET) $(STAGE?) $(DATASET?) my_output.out < $(TEST?) > my_output.stdout.out

# Test Searching 

test_stage1:
	./$(DICT1_TARGET) $(STAGE1) tests/dataset_full.csv my_outputs/testfull.out < tests/testfull.in > my_outputs/testfull.stdout.out
	diff my_outputs/testfull.out matching_results/testfull.out > my_outputs/diff_out || true
	diff my_outputs/testfull.stdout.out matching_results/testfull.stdout.out > my_outputs/diff_stdout || true
test_stage2:
	./$(DICT2_TARGET) $(STAGE2) tests/dataset_del.csv my_outputs/delfull.out < tests/del3.in > my_outputs/delfull.stdout.out
	diff my_outputs/delfull.out matching_results/del3.out > my_outputs/diff_out || true
	diff my_outputs/delfull.stdout.out matching_results/del3.stdout.out > my_outputs/diff_stdout || true
	
test_del:
	./$(DICT2_TARGET) $(STAGE2) tests/dataset_del.csv my_outputs/delfull.out < tests/del1.in > my_outputs/delfull.stdout.out


