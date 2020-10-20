# Make file for soyshell

CC := cc
COMMANDS := $(wildcard src/commands/*.c)

.PHONY: all commands clean

all: soyshell commands

soyshell: src/Parser.o src/main.o
	@${CC} -O2 -o soyshell src/main.o src/Parser.o

commands: # Compile the binaries for all the commands and store them in bin folder
	@$(foreach c, $(COMMANDS), \
		$(eval nodir = $(notdir $(c))) \
		$(eval base = $(basename $(nodir))) \
		${CC} -o bin/$(base) -O2 $(c); \
	)

src/main.o: src/main.c src/Parser.h
	@${CC} -c -O2 src/main.c -o src/main.o

src/Parser.o: src/Parser.c src/Parser.h
	@${CC} -c -O2 src/Parser.c -o src/Parser.o

clean:
	@rm ./src/*.o
