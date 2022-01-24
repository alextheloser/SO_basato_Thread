#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAX 4316

typedef struct {
    int x;
    int y;
    int idNemico;
    pthread_t Tnemico;
}valuesNemici;

void *navicella();
void* nemici(void *arg);
void *controllo();
int menu(int maxx, int maxy);


int maxx, maxy, numNemici;

sem_t piene, libere;


int main() {
    int filedes[2], i, j=0, x_nemici, y_nemici, numColonne=1;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, 1); //funzione per leggere i tasti della tastiera.
    curs_set(0);

    //impostazioni dei colori.
    start_color();
    init_color(80, 255, 0, 0);
    init_pair(1,COLOR_WHITE,COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(1));

    //viene presa la dimensione delo schermo.
    getmaxyx(stdscr, maxy, maxx);
    x_nemici=maxx-7;
    y_nemici=3;

    //menù iniziale.
    int valoreDifficolta = menu(maxx,maxy);

    clear();
    refresh();

    //in base al valore di ritorno del menù viene impostato il numero di nemici.
    switch(valoreDifficolta){
        case 0:
            numNemici=5;
            break;

        case 1:
            numNemici=10;
            break;

        case 2:
            numNemici=15;
            break;

        case 3:
            numNemici=0;
            break;
    }

    getmaxyx(stdscr, maxy, maxx);
    x_nemici=maxx-5;
    y_nemici=3;

    refresh();

    sem_init(&piene,0,0);
    sem_init(&libere,0,MAX);

    //Creare questa roba in un loop con tante variabili per ogni nemico
    pthread_t Tnavicella;
    pthread_create(&Tnavicella,NULL,navicella,NULL);
    //

    valuesNemici nemico[numNemici];
    pthread_t Tnemico[numNemici];
    //pthread_create(&Tnemico,NULL,nemiciPrimoLivello,(void *)&nemico1);

    //generazione dei thread che si occupano della generazione delle coordinate dei nemici.
    for(i=0; i<numNemici; i++){

        nemico[i].x = x_nemici;
        nemico[i].y = y_nemici;
        nemico[i].idNemico=i;
        nemico[i].Tnemico=Tnemico[i];

        pthread_create(&Tnemico[i],NULL,nemici,(void *)&nemico[i]);

        j++;
        //aggiornamento delle coordinate iniziali del prossimo nemico da generare.
        y_nemici=y_nemici+15;
        if(j>3 || y_nemici+7>maxy){
            j=0;
            x_nemici-=9;
            if(numColonne%2==0)
                y_nemici=3;
            else
                y_nemici=10;
            numColonne++;
        }


    }

    controllo();

    pthread_detach(Tnavicella);

    for(i=0; i<numNemici; i++) {
        pthread_detach(Tnemico[i]);
    }
    clear();

    endwin();
    exit(0);
}
