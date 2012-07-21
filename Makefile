all: main.o Entity.o Keyboard.o Draw.o Pushdown.o GameLogic.o GameScreens.o Particle.o Audio.o
	gcc -Wall -o whimsy.exe main.o Entity.o Keyboard.o Draw.o Pushdown.o GameLogic.o Particle.o GameScreens.o Audio.o -lmingw32 -lSDLmain -lSDL -lSDL_ttf -lSDL_image -lSDL_mixer

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

GameLogic.o:
	gcc -g -Wall -c GameLogic.c

GameScreens.o:
	gcc -g -Wall -c GameScreens.c

Particle.o:
	gcc -g -Wall -c Particle.c

Audio.o:
	gcc -g -Wall -c Audio.c

clean:
	rm main.o Entity.o Keyboard.o Draw.o Pushdown.o GameLogic.o GameScreens.o Particle.o Audio.o whimsy.exe
