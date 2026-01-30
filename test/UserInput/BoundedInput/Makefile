BINARY:=bounded-input-test.out
OBJECT_DIR:=./objects
TEST_CODE_DIR:=./source
SOURCE_CODE_DIR:=../../../source
INCLUDES:=-I../../../includes -I./test_includes

CC:=gcc
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes \
		  -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings

TEST_C_FILES:=$(wildcard $(TEST_CODE_DIR)/*.c)
SOURCE_C_FILES:=$(SOURCE_CODE_DIR)/userInput.c

O_FILES := \
  $(patsubst $(TEST_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(TEST_C_FILES)) \
  $(patsubst $(SOURCE_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCE_C_FILES))

all: $(BINARY)

$(BINARY): $(O_FILES)
	$(CC) $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) -o $@ $^ -lcmocka

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OBJECT_DIR)/%.o: $(TEST_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_CODE_DIR)/%.c | $(OBJECT_DIR)
	$(CC) -c $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) $< -o $@

run:
	./$(BINARY)

clean:
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(OBJECT_DIR)/*.d
	rm -f $(BINARY)
	rm -f ./*.d
