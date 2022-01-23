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
    int x;
    int y;
    int idNemico;
    pthread_t Tnemico;
}valuesNemici;

typedef struct {
    int x_bomba;
    int y_bomba;
    int id;
    pthread_t threaddino;
    identity i;
}valuesBomba;

void* nemici(void *arg);
void *bomba(void *arg);