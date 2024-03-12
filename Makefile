CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -Wpedantic -std=c17 -g -lm

SRC = src/
BUILD = build/
INC = include/

TARGET = main
OBJECTS = $(patsubst %.c,%.o,$(addprefix $(BUILD),$(notdir $(wildcard $(SRC)*.c))))
DEPENDS = $(wildcard $(BUILD)*.d)

TARGET := $(BUILD)$(TARGET)

.PHONY: all clean rebuild run test
.SILENT: run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

$(BUILD)%.o: $(SRC)%.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

-include $(DEPENDS)
 
clean:
	rm $(DEPENDS) $(OBJECTS) $(TARGET)

rebuild: clean all

run: all 
	$(TARGET)
