SpaceDefender: main.c gestioneBuffer.o gestioneOggetti.o
	gcc -o SpaceDefender main.c gestioneBuffer.o gestioneOggetti.o -lncurses -lpthread

gestioneOggetti.o: gestioneOggetti.c gestioneOggetti.h
	gcc -c -o navicella.o navicella.c

gestioneBuffer.o: gestioneBuffer.c gestioneBuffer.h
	gcc -c -o controllo.o controllo.c

clean:
	rm *.o
	rm SpaceDefender
