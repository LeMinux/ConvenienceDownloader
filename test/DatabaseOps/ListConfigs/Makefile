BINARY:=list_configs_unit_test.out
OBJECT_DIR:=./test_objects
TEST_CODE_DIR:=./test_source
SOURCE_CODE_DIR:=../../../source
COMMON_CODE_DIR:=../menu_source/
INCLUDES:=-I../../../includes -I./test_includes

CC:=gcc
COMPILER_FLAG:=-DTESTING
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes \
		  -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings


COMMON_C_FILES:=$(wildcard $(COMMON_CODE_DIR)/*.c)
TEST_C_FILES:=$(wildcard $(TEST_CODE_DIR)/*.c)
SOURCE_C_FILES:=$(SOURCE_CODE_DIR)/databaseOps.c $(SOURCE_CODE_DIR)/fileOps.c $(SOURCE_CODE_DIR)/databaseUserInput.c $(SOURCE_CODE_DIR)/userInput.c

O_FILES := \
  $(patsubst $(TEST_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(TEST_C_FILES)) \
  $(patsubst $(COMMON_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(COMMON_C_FILES)) \
  $(patsubst $(SOURCE_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCE_C_FILES))

all: $(BINARY)

$(BINARY): $(O_FILES)
	$(CC) -g $(COMPILER_FLAG) $(MOCK_FLAGS) $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) -o $@ $^ -lcmocka -lsqlite3

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OBJECT_DIR)/%.o: $(COMMON_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(COMPILER_FLAG) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: $(TEST_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(COMPILER_FLAG) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) $(COMPILER_FLAG) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

run:
	./$(BINARY)

clean:
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(OBJECT_DIR)/*.d
	rm -f ./*.d
	rm -f $(BINARY)
