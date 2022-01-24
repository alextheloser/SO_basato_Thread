SpaceDefender: main.c gestioneBuffer.o gestioneOggetti.o
	gcc -o SpaceDefender main.c gestioneBuffer.o gestioneOggetti.o -lncurses -lpthread

gestioneOggetti.o: gestioneOggetti.c gestioneOggetti.h
	gcc -c -o gestioneOggetti.o gestioneOggetti.c

gestioneBuffer.o: gestioneBuffer.c gestioneBuffer.h
	gcc -c -o gestioneBuffer.o gestioneBuffer.c

clean:
	rm *.o
	rm SpaceDefender
