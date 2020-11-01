EXEC = \
	simple-shell\

LIBS = -lreadline

SRC = \
	simple-shell.c\

build:
	gcc -g -Wall -Wpedantic $(LIBS) $(SRC) -o $(EXEC) && chmod +x $(EXEC)

run: build
	./$(EXEC)

debug: build
	gdb $(EXEC)

