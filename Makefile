CC = gcc
COPS = -Wall -Iinclude

SRC := src
BUILD := build

all: main

clean :
	rm -rf $(BUILD)

$(BUILD)/%_c.o: $(SRC)/%.c
	mkdir -p $(@D)
	$(CC) $(COPS) -c $< -o $@

C_FILES = $(wildcard $(SRC)/*.c)
OBJ_FILES = $(C_FILES:$(SRC)/%.c=$(BUILD)/%_c.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

main: $(OBJ_FILES)
	$(CC) $(COPS) -o $@ $(OBJ_FILES)
