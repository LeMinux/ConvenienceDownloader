BINARY:=user-input-test.out
OBJECT_DIR:=./test_objects
TEST_CODE_DIR:=./test_source

CC:=gcc
DEP_FLAGS :=-MP -MD
C_FLAGS :=-Wall -Werror -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings
MOCK_FLAGS:=-Wl --wrap=stubbed

C_FILES:=$(wildcard $(TEST_CODE_DIR)/*.c) ../../source/userInput.c
O_FILES := $(patsubst %.c, %.o, $(C_FILES))
DEP_FILES :=$(patsubst %.c, %.d, $(C_FILES))

all: $(BINARY)

$(BINARY): $(O_FILES)
	$(CC) $(C_FLAGS) $(DEP_FLAGS) $(MOCK_FLAGS) -I../../includes/ -o $@ $^ -lcmocka

$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

%.o : %.c | $(OBJECT_DIR)
	$(CC) -c $(C_FLAGS) $< -o $(OBJECT_DIR)/$(notdir $@)

#$(OBJECT_DIR)/%.o : ../../source/%.c# | $(OBJECT_DIR)
#	$(CC) -c $(C_FLAGS) $< -o $@

run:
	./$(BINARY)
