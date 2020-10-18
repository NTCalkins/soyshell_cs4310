make:
	g++ -o bin/ls src/commands/ls.c
	g++ -o bin/cp src/commands/cp.c
	g++ -o bin/mkdir src/commands/mkdir.c
	g++ -o bin/rm src/commands/rmdir.c
	g++ -o bin/pwd src/commands/pwd.c
	g++ -o main src/Parser.c
	./main
 clean:
	rm bin/*
	rm main