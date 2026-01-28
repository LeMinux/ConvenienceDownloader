BINARY:=update_menu_integration_test.out
OBJECT_DIR:=./objects
TEST_CODE_DIR:=./source
SOURCE_CODE_DIR:=../../source
INCLUDES:=-I../../includes -I./includes

CC:=gcc
COMPILER_FLAG:=-DTESTING
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Wextra -Wpedantic -Wshadow -Wstrict-prototypes \
		  -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings
MOCK_FLAGS:=-Wl,--wrap=boundedInput,--wrap=takeIndexInput,--wrap=takeDirectoryInput,--wrap=takeDepthInput

DIR_ENV:=./dir_env
ROOT1:=$(DIR_ENV)/root1
ROOT_FILE:=$(ROOT1)/file
ROOT_LINK:=$(ROOT1)/link

LEFT_DIR:=$(ROOT1)/left_dir
LEFT_DIR_LEFT:=$(LEFT_DIR)/inner_left
LEFT_DIR_RIGHT:=$(LEFT_DIR)/inner_right
LEFT_DIR_MOST_INNER:=$(LEFT_DIR_RIGHT)/inner_inner_dir

RIGHT_DIR:=$(ROOT1)/right_dir
RIGHT_DIR_LEFT:=$(RIGHT_DIR)/inner_left
RIGHT_DIR_RIGHT:=$(RIGHT_DIR)/inner_right
RIGHT_DIR_MOST_INNER:=$(RIGHT_DIR_LEFT)/inner_inner_dir

ROOT2:=$(DIR_ENV)/root2


TEST_C_FILES:=$(wildcard $(TEST_CODE_DIR)/*.c)
SOURCE_C_FILES:=$(SOURCE_CODE_DIR)/databaseUserInput.c $(SOURCE_CODE_DIR)/databaseOps.c $(SOURCE_CODE_DIR)/userInput.c $(SOURCE_CODE_DIR)/fileOps.c $(SOURCE_CODE_DIR)/testWrapInput.c $(SOURCE_CODE_DIR)/testSetups.c

O_FILES := \
  $(patsubst $(TEST_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(TEST_C_FILES)) \
  $(patsubst $(SOURCE_CODE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCE_C_FILES))

all: $(BINARY)

$(DIR_ENV):
	mkdir -p $(ROOT1)
	touch $(ROOT_FILE)
	ln -sf ../root2 $(ROOT_LINK)

	mkdir $(LEFT_DIR)
	mkdir $(LEFT_DIR_LEFT)
	mkdir $(LEFT_DIR_RIGTH)
	mkdir $(LEFT_DIR_MOST_INNER)

	mkdir $(RIGHT_DIR)
	mkdir $(RIGHT_DIR_LEFT)
	mkdir $(RIGHT_DIR_RIGTH)
	mkdir $(RIGHT_DIR_MOST_INNER)

	mkdir $(ROOT2)

$(BINARY): $(O_FILES) | $(DIR_ENV)
	$(CC) -g $(COMPILER_FLAG) $(MOCK_FLAGS) $(C_FLAGS) $(DEP_FLAGS) $(INCLUDES) -o $@ $^ -lcmocka -lsqlite3

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

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


