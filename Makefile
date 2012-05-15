all: main.o Entity.o
	gcc -Wall -o whimsy.exe main.o Entity.o -lmingw32 -lSDLmain -lSDL

main.o:
	gcc -g -Wall -c main.c
	
Entity.o:
	gcc -g -Wall -c Entity.c
	
clean:
	rm main.o Entity.o
