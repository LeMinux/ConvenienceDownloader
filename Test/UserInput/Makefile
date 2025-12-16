BINARY:=user-input-test.out
OBJECT_DIR:=./objects/

CC:=gcc
DEP_FLAGS :=-MP -MD
C_FLAGS:=-Wall -Werror -Wextra -Wpedantic -Wshadow -Wstrict-prototypes -Wmissing-prototypes -Wstrict-overflow=5 -Wwrite-strings $(DEP_FLAGS)

C_FILES:=./main.c ./user_input_unit_test.c ../../source/userInput.c
O_FILES := $(patsubst %.c,$(OBJECT_DIR)/%.o,$(notdir $(C_FILES)))

all: $(BINARY)

$(BINARY): $(O_FILES)
	$(CC) $(C_FLAGS) -o $@ $^ -lcmocka


$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

$(OBJECT_DIR)/%.o : %.c | $(OBJECT_DIR)
	$(CC) -c $(C_FLAGS) $< -o $@

$(OBJECT_DIR)/%.o : ../../source/%.c | $(OBJECT_DIR)
	$(CC) -c $(C_FLAGS) $< -o $@

run:
	./$(BINARY)
