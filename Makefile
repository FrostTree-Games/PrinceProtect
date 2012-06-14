all: main.o Entity.o Keyboard.o Draw.o Pushdown.o
	gcc -Wall -o whimsy.exe main.o Entity.o Keyboard.o Draw.o Pushdown.o -lmingw32 -lSDLmain -lSDL -lSDL_ttf

main.o:
	gcc -g -Wall -c main.c
	
Entity.o:
	gcc -g -Wall -c Entity.c
	
Keyboard.o:
	gcc -g -Wall -c Keyboard.c

Draw.o:
	gcc -g -Wall -c Draw.c

Pushdown.o:
	gcc -g -Wall -c Pushdown.c

clean:
	rm main.o Entity.o Keyboard.o Draw.o Pushdown.o
