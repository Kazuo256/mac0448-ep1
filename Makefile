PROG_NAME = ep1

SRC_DIR = src
OBJ_DIR = .temp
#OUTPUT_DIR = bin

CC = gcc
CFLAGS = -I$(SRC_DIR) -ansi -pedantic -Wall -O2 $(FLAGS)

#OUTPUT = bin/$(PROG_NAME)
OUTPUT = $(PROG_NAME)

include objs.makefile

#$(PROG_NAME): $(OBJ_DIR) $(OUTPUT_DIR) $(OBJS)
$(PROG_NAME): $(OBJ_DIR) $(OBJ_DIRS) $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(OUTPUT)

$(OBJ_DIR):
	mkdir $@

#$(OUTPUT_DIR):
#	mkdir $@

.temp/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

include deps.makefile

.PHONY: debug
debug: CFLAGS += -g -DEP1_DEBUG
debug: $(PROG_NAME)

.PHONY: clean
clean:
	rm -rf $(OUTPUT)
	rm -rf $(OBJ_DIR)


