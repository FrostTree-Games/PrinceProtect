all: main.o Entity.o Keyboard.o
	gcc -Wall -o whimsy.exe main.o Entity.o Keyboard.o -lmingw32 -lSDLmain -lSDL

main.o:
	gcc -g -Wall -c main.c
	
Entity.o:
	gcc -g -Wall -c Entity.c
	
Keyboard.o:
	gcc -g -Wall -c Keyboard.c
	
clean:
	rm main.o Entity.o Keyboard.o
