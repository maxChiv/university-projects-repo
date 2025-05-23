EXE = translate
CC = gcc
CFLAGS = -O3 -Wall -g -I$(INC_DIR)  # Add -I flag for include directory
GDBFLAG = -O0 # Used if needing to get extra compilation data for debugging

SRC_DIR = src
OBJ_DIR = obj
LL_DIR = linked-list
INC_DIR = include  # Directory where header files are located

# All .c files in the src directory, and nested directories.
SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/$(LL_DIR)/*.c)  
	# Must be changed if there are some to be left out
# All .o files in the obj, and nested directories
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compiling each .c into its corresponding .o, ensuring obj dir exists.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)	
	$(CC) $(CFLAGS) -c $< -o $@

# TESTING
# Task 1 tests
test-task1: $(EXE)
	./$(EXE) -f cases/task1/spec.txt -t task1 | diff - cases/task1/spec.out
	./$(EXE) -f cases/task1/spec-mask.txt -t task1 | diff - cases/task1/spec-mask.out
	./$(EXE) -f cases/task1/bound.txt -t task1 | diff - cases/task1/bound.out

# Task 2 tests
test-task2: $(EXE)
	./$(EXE) -f cases/task1/spec.txt -t task2 | diff - cases/task2/spec.out
	./$(EXE) -f cases/task1/spec-mask.txt -t task2 | diff - cases/task2/spec-mask.out
	./$(EXE) -f cases/task1/bound.txt -t task2 | diff - cases/task2/bound.out
	./$(EXE) -f cases/task2/same.txt -t task2 | diff - cases/task2/same.out
	./$(EXE) -f cases/task2/3page.txt -t task2 | diff - cases/task2/3page.out
	./$(EXE) -f cases/task2/255.txt -t task2 | diff - cases/task2/255.out
	./$(EXE) -f cases/task2/255-more.txt -t task2 | diff - cases/task2/255-more.out

# Task 3 tests
test-task3: $(EXE)
	./$(EXE) -f cases/task3/spec2.txt -t task3 | diff - cases/task3/spec2.out
	./$(EXE) -f cases/task3/512.txt -t task3 | diff - cases/task3/512.out
	./$(EXE) -f cases/task3/loop.txt -t task3 | diff - cases/task3/loop.out

# Task 4 tests
test-task4: $(EXE)
	./$(EXE) -f cases/task4/basic.txt -t task4 | diff - cases/task4/basic.out
	./$(EXE) -f cases/task2/3page.txt -t task4 | diff - cases/task4/3page.out
	./$(EXE) -f cases/task4/lru.txt -t task4 | diff - cases/task4/lru.out
	./$(EXE) -f cases/task3/spec2.txt -t task4 | diff - cases/task4/spec2.out

# My test 
test-my: $(EXE)
	./$(EXE) -f cases/mytests/mytest1.txt -t task4 

# All tests run
test-all: test-task1 test-task2 test-task3 test-task4

# Hidden Task 1 tests
test-hidden-task1: $(EXE)
	./$(EXE) -f cases.hidden/task1/bound2.txt -s cases.hidden/task1/bound2.spec -t task1 | diff - cases.hidden/task1/bound2.out
	./$(EXE) -f cases.hidden/task1/max.txt -s cases.hidden/task1/max.spec -t task1 | diff - cases.hidden/task1/max.out
	./$(EXE) -f cases.hidden/task1/rand-255.txt -t task1 | diff - cases.hidden/task1/rand-255.out

# Hidden Task 2 tests
test-hidden-task2: $(EXE)
	./$(EXE) -f cases.hidden/task2/255-more-extended.txt -t task2 | diff - cases.hidden/task2/255-more-extended.out
	./$(EXE) -f cases.hidden/task2/256.txt -t task2 | diff - cases.hidden/task2/256.out
	./$(EXE) -f cases.hidden/task2/3page-shuf.txt -t task2 | diff - cases.hidden/task2/3page-shuf.out
	./$(EXE) -f cases.hidden/task2/rand-255-nomask.txt -t task2 | diff - cases.hidden/task2/rand-255-nomask.out
	./$(EXE) -f cases.hidden/task2/rand-fill.txt -t task2 | diff - cases.hidden/task2/rand-fill.out
	./$(EXE) -f cases.hidden/task2/same2.txt -t task2 | diff - cases.hidden/task2/same2.out

# Hidden Task 3 tests
test-hidden-task3: $(EXE)
	./$(EXE) -f cases.hidden/task3/1024.txt -t task3 | diff - cases.hidden/task3/1024.out
	./$(EXE) -f cases.hidden/task3/repeat-513.txt -t task3 | diff - cases.hidden/task3/repeat-513.out
	./$(EXE) -f cases.hidden/task3/spec2-extended.txt -t task3 | diff - cases.hidden/task3/spec2-extended.out

# Hidden Task 4 tests
test-hidden-task4: $(EXE)
	./$(EXE) -f cases.hidden/task4/42-pages.txt -t task4 | diff - cases.hidden/task4/42-pages.out
	./$(EXE) -f cases.hidden/task4/500.txt -t task4 | diff - cases.hidden/task4/500.out
	./$(EXE) -f cases.hidden/task4/1000.txt -t task4 | diff - cases.hidden/task4/1000.out
	./$(EXE) -f cases.hidden/task4/10000.in -t task4 | diff - cases.hidden/task4/10000-3.out
	./$(EXE) -f cases.hidden/task4/10000.in -t task4 | diff - cases.hidden/task4/10000-4.out
	./$(EXE) -f cases.hidden/task4/3page-shuf.txt -t task4 | diff - cases.hidden/task4/3page-shuf.out
	./$(EXE) -f cases.hidden/task4/spec2-extended.txt -t task4 | diff - cases.hidden/task4/spec2-extended.out

# All hidden tasks
test-hidden-all: test-hidden-task1 test-hidden-task2 test-hidden-task3 test-hidden-task4

#OTHER
format:
	clang-format -style=file -i $(SRC_DIR)/*.c

clean:
	@rm -rf $(OBJ_DIR)  # Removes obj directory and everything inside it
	@rm -f $(EXE)       # Removes the executable
