CC = gcc

CFLAGS = -Wall -Wextra -Werror -pedantic -I$(SOURCE_PATH)

RM = rm -f

SOURCE_PATH = sources
OBJECT_PATH = runfiles
INCLUDE_PATH = headers

SOURCES = $(wildcard $(SOURCE_PATH)/*.c)
HEADERS = $(wildcard $(INCLUDE_PATH)/*.h)
OBJECTS = $(subst sources/,runfiles/,$(subst .c,.o,$(SOURCES)))

OBJECTS_F = main.o variables.o pipes.o ifelse.o basic_command.o
OBJ_FILES = $(addprefix $(OBJECT_PATH)/, $(OBJECTS_F))

.PHONY: all default clean

all: myshell

default: all


myshell: $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJECT_PATH)/%.o: $(SOURCE_PATH)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECT_PATH)/*.o *.so myshell
