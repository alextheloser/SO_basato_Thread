#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>

//dimensione del buffer.
#define MAX 4316

//thread-argument per le funzione nemici.
typedef struct {
    int x;
    int y;
    int idNemico;
    pthread_t Tnemico;
}valuesNemici;

//prototipi delle funzioni usate nel file.
void *navicella();
void* nemici(void *arg);
void *controllo();
int menu(int maxx, int maxy);

//variabili per la dimensione dello schermo e numero di nemici.
int maxx, maxy, numNemici;
//semafori usati per la gestione buffer.
sem_t piene, libere;

/**
 * Funzione principale che si occupa di creare i threads.
 */
int main() {

    int i, j=0, x_nemici, y_nemici, numColonne=1;

    //impostazioni per lo schermo
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

    //acquisizione dimensione dello schermo.
    getmaxyx(stdscr, maxy, maxx);
    x_nemici=maxx-5;
    y_nemici=3;

    refresh();

    //inizializzazione dei semafori.
    sem_init(&piene,0,0);
    sem_init(&libere,0,MAX);

    //creazione thread che gestisce la navicella.
    pthread_t Tnavicella;
    pthread_create(&Tnavicella,NULL,navicella,NULL);

    valuesNemici nemico[numNemici];
    pthread_t Tnemico[numNemici];

    //generazione dei thread che si occupano della generazione delle coordinate dei nemici.
    for(i=0; i<numNemici; i++){

        //assegno al thread-argument i suoi valori.
        nemico[i].x = x_nemici;
        nemico[i].y = y_nemici;
        nemico[i].idNemico=i;
        nemico[i].Tnemico=Tnemico[i];

        //creo il thread che gestisce il nemico
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

    //il thread principale entra nella funzione controllo.
    controllo();
}
