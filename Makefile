BIN_DIR = bin
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

TARGET = $(BIN_DIR)/fat12-file-system
SRC = $(wildcard $(SRC_DIR)/*.c)
INCLUDE = -I ./$(INC_DIR)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
CC = gcc
FLAGS = -g

TEST_BIN_DIR = test/bin
TEST_SRC_DIR = test/src
TEST_BUILD_DIR = test/build

TEST_TARGET = $(TEST_BIN_DIR)/gtest
TEST_SRC = $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_OBJ = $(TEST_SRC:$(TEST_SRC_DIR)/%.cpp=$(TEST_BUILD_DIR)/%.o)
TEST_CC = g++
TEST_FLAGS = -lgtest -lpthread

TARGETS = $(TARGET) $(TEST_TARGET)

proj: $(TARGET)
all: $(TARGETS)

$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(INCLUDE) $(FLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJ) $(filter-out $(BUILD_DIR)/fat12-file-system.o, $(OBJ))
	@mkdir -p $(TEST_BIN_DIR)
	$(TEST_CC) $^ -o $@ $(TEST_FLAGS)

$(TEST_BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	@mkdir -p $(TEST_BUILD_DIR)
	$(TEST_CC) $(INCLUDE) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(BUILD_DIR)
	rm -rf $(TEST_BIN_DIR)
	rm -rf $(TEST_BUILD_DIR)

run-test:
	$(MAKE) all
	./test/bin/gtest
