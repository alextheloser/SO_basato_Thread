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

typedef enum {Navicella, Nemico, Missile, Bomba, BombaAvanzata}identity;

typedef struct{
    int x;
    int y;
    identity i;
    int id; //solo per i nemici
    pthread_t Tthreadtokill;
}Position;


typedef struct {
    int navx;
    int navy;
    int diry;
    pthread_t Tmissile;
}valuesMissili;

void *navicella();
void *missile(void *arg);


