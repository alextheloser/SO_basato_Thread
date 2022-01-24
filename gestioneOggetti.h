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

#define PASSO 1

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

typedef struct {
    int navx;
    int navy;
    int diry;
    pthread_t Tmissile;
}valuesMissili;

void *navicella();
void *missile(void *arg);
void* nemici(void *arg);
void *bomba(void *arg);
void *controllo();
void scriveNelBuffer(Position oggetto);
Position leggeDalBuffer();

char SpriteNavicella[6][6]={
        "<-\\",
        "~[-->",
        "<-/"};

char SpriteNavicellaDmgd2[6][6]={
        "<X\\",
        "-[-->",
        "<X/"};

char SpriteNavicellaDmgd3[6][6]={
        "<X\\",
        ":[XX>",
        "<X/"};

char SpriteNavicellaMorta[6][6]={
        "XX\\",
        "XXXXx",
        "XX/"};

char SpriteNemicoBase[4][4]={
        " /\\",
        "<O(",
        " \\/"};

char SpriteNemicoMorente[4][4]={
        " xx",
        "xxx",
        " xx"};

char SpriteNemicoAvanzato[4][4]={
        " /",
        "<:(",
        " \\"};

char SpriteMissile='>';

char SpriteBomba='@';

char logo[8][150]={".d88888b                                      888888ba           .8888b                         dP                            ",
                   "88.    \"'                                     88    `8b          88   \"                         88                            ",
                   "`Y88888b. 88d888b. .d8888b. .d8888b. .d8888b. 88     88 .d8888b. 88aaa  .d8888b. 88d888b. .d888b88 .d8888b. 88d888b. .d8888b. ",
                   "      `8b 88'  `88 88'  `88 88'  `\"\" 88ooood8 88     88 88ooood8 88     88ooood8 88'  `88 88'  `88 88ooood8 88'  `88 Y8ooooo. ",
                   "d8'   .8P 88.  .88 88.  .88 88.  ... 88.  ... 88    .8P 88.  ... 88     88.  ... 88    88 88.  .88 88.  ... 88             88 ",
                   " Y88888P  88Y888P' `88888P8 `88888P' `88888P' 8888888P  `88888P' dP     `88888P' dP    dP `88888P8 `88888P' dP       `88888P' ",
                   "          88                                                                                                                  ",
                   "          dP                                                                                                                  "};

char gameover[7][100]={"  .oooooo.                                               .oooooo.                                  ",
                       " d8P'  `Y8b                                             d8P'  `Y8b                                 ",
                       "888            .oooo.   ooo. .oo.  .oo.    .ooooo.     888      888 oooo    ooo  .ooooo.  oooo d8b ",
                       "888           `P  )88b  `888P\"Y88bP\"Y88b  d88' `88b    888      888  `88.  .8'  d88' `88b `888\"\"8P ",
                       "888     ooooo  .oP\"888   888   888   888  888ooo888    888      888   `88..8'   888ooo888  888     ",
                       "`88.    .88'  d8(  888   888   888   888  888    .o    `88b    d88'    `888'    888    .o  888     ",
                       " `Y8bood8P'   `Y888\"\"8o o888o o888o o888o `Y8bod8P'     `Y8bood8P'      `8'     `Y8bod8P' d888b    "};

char youwon[7][100]={"oooooo   oooo                          oooooo   oooooo     oooo                        .o. ",
                     " `888.   .8'                            `888.    `888.     .8'                         888 ",
                     "  `888. .8'    .ooooo.  oooo  oooo       `888.   .8888.   .8'    .ooooo.  ooo. .oo.   888 ",
                     "   `888.8'    d88' `88b `888  `888        `888  .8'`888. .8'    d88' `88b `888P\"Y88b   Y8P ",
                     "    `888'     888   888  888   888         `888.8'  `888.8'     888   888  888   888   `8' ",
                     "     888      888   888  888   888          `888'    `888'      888   888  888   888   .o. ",
                     "    o888o     `Y8bod8P'  `V88V\"V8P'          `8'      `8'       `Y8bod8P' o888o o888o  Y8P"};

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
int isMissileVivo1=0, isMissileVivo2=0, maxx, maxy, valoreDifficolta=1, numNemici, turnodimorireNemici=-1, turnodimorireNavicella=-1, rindondanzaTurnodiMorireBomba=-1, rindondanzaTurnodiMorireBombaAvanzata=-1;
pthread_t turnodimorire=-1, turnodimorireBomba=-1;