all: main.o
	gcc -Wall -o whimsy.exe main.o -lmingw32 -lSDLmain -lSDL

main.o:
	gcc -g -Wall -c main.c
	
clean:
	rm main.o
