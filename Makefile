make:
	g++ -o bin/ls src/ls.c
	g++ -o bin/cp src/cp.c
	g++ -o bin/mkdir src/mkdir.c
	g++ -o bin/rmdir src/rmdir.c
	g++ -o bin/pwd src/pwd.c
	g++ -o main src/Parser.c
	./main
 clean:
	rm bin/*
	rm main