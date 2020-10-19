# Make file for soyshell

CC := cc
COMMANDS := $(wildcard src/commands/*.c)

.PHONY: all commands clean

all: soyshell commands

soyshell: main.o Parser.o
	@${CC} -o -O2 soyshell ./src/main.o ./src/Parser.o

commands: # Compile the binaries for all the commands and store them in bin folder
	@$(foreach c, $(COMMANDS), \
		$(eval nodir = $(notdir $(c))) \
		$(eval base = $(basename $(nodir))) \
		${CC} -o bin/$(base) -O2 $(c); \
		)
Parser.o:
	@${CC} -c -O2 ./src/Parser.c

main.o:
	@${CC} -c -O2 ./src/main.c

clean:
	@rm ./src/*.o
