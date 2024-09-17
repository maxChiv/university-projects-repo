# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Source files and target
DICT3_TARGET = dict3
DICT3_SRC = driver.c dict.c io.c linkedlist.c data.c comparison.c radix.c bitoperations.c
DICT3_OBJ = $(DICT3_SRC:.c=.o)

# Source files and target
DICT4_TARGET = dict4
DICT4_SRC = driver.c dict.c io.c linkedlist.c data.c comparison.c radix.c bitoperations.c
DICT4_OBJ = $(DICT4_SRC:.c=.o)

# Stages for input
STAGE3 = 3
STAGE4 = 4

# Explicit dependencies
driver.o: linkedlist.h data.h io.h comparison.h
dict.o: linkedlist.h data.h io.h comparison.h radix.h
io.o: linkedlist.h io.h comparison.h
linkedlist.o: linkedlist.h comparison.h
comparison.o: comparison.h
data.o: comparison.h
radix.o: radix.h linkedlist.h comparison.h bitoperations.h
bitoperations.o: bitoperations.h


# Rule to build the object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build dict3
$(DICT3_TARGET): $(DICT3_OBJ)
	$(CC) $(CFLAGS) $(DICT3_OBJ) -o $(DICT3_TARGET)

# Rule to build dict3
$(DICT4_TARGET): $(DICT4_OBJ)
	$(CC) $(CFLAGS) $(DICT4_OBJ) -o $(DICT4_TARGET)


# Rule to run dict3 with Valgrind
valgrind_dict3: $(DICT3_TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(DICT3_TARGET) $(STAGE3) tests/dataset_full.csv my_outputs/testfull.out < tests/testfull.in > my_outputs/testfull.stdout.out


# Rule to run dict3 with GDB
gdb_dict3: $(DICT3_TARGET)
	gdb ./$(DICT3_TARGET)

# Rule to run dict4 with GDB
gdb_dict4: $(DICT4_TARGET)
	gdb ./$(DICT4_TARGET)


# Clean up build artifacts
clean:
	rm -f $(DICT3_OBJ) $(DICT3_TARGET)
	rm -f $(DICT4_OBJ) $(DICT4_TARGET)

# Testing ----/
# example
# ./$(DICT?_TARGET) $(STAGE?) $(DATASET?) my_output.out < $(TEST?) > my_output.stdout.out

# Test Searching 
test15_dict3: 
	./$(DICT3_TARGET) $(STAGE3) tests/dataset_15.csv my_outputs/output.out < tests/test15.in > my_outputs/output.stdout.out
test100_dict3:
	./$(DICT3_TARGET) $(STAGE3) tests/dataset_100.csv my_outputs/output.out < tests/test100.in > my_outputs/output.stdout.out
test1000_dict3:
	./$(DICT3_TARGET) $(STAGE3) tests/dataset_1000.csv my_outputs/output.out < tests/test1000.in > my_outputs/output.stdout.out
testfull_dict3:
	./$(DICT3_TARGET) $(STAGE3) tests/dataset_full.csv my_outputs/output.out < tests/testfull.in > my_outputs/output.stdout.out

# Test Radix 
test15_dict4: 
	./$(DICT4_TARGET) $(STAGE4) tests/dataset_15.csv my_outputs/output.out < tests/test15.in > my_outputs/output.stdout.out
test100_dict4:
	./$(DICT4_TARGET) $(STAGE4) tests/dataset_100.csv my_outputs/output.out < tests/test100.in > my_outputs/output.stdout.out
test1000_dict4:
	./$(DICT4_TARGET) $(STAGE4) tests/dataset_1000.csv my_outputs/output.out < tests/test1000.in > my_outputs/output.stdout.out
testfull_dict4:
	./$(DICT4_TARGET) $(STAGE4) tests/dataset_full.csv my_outputs/output.out < tests/testfull.in > my_outputs/output.stdout.out
