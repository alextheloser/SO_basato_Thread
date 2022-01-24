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

typedef enum {Navicella, Nemico, Missile, Bomba, BombaAvanzata}identity;

typedef struct{
    int x;
    int y;
    identity i;
    int id; //solo per i nemici
    pthread_t Tthreadtokill;
}Position;

void scriveNelBuffer(Position oggetto);
Position leggeDalBuffer();

pthread_mutex_t mtxBuffer = PTHREAD_MUTEX_INITIALIZER;
sem_t piene, libere;
Position buffer[MAX];
int posLettura =0, posScrittura =0;