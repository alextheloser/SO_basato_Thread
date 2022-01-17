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
#define MAX 4316


typedef enum {Navicella, Nemico, Missile, Bomba}identity;

typedef struct{
    int x;
    int y;
    identity i;
    int id; //solo per i nemici
    pthread_t Tthreadtokill;
}Position;


int menu();
void *navicella();
void *nemiciPrimoLivello(void *arg);
void nemiciSecondoLivello(int pipeout, int x, int y, int idNemico, int maxx, int maxy);
void *controllo();
void *missile(void *arg);
void *bomba(void *arg);

int numNemici=1;
int nemiciVivi;

//Position pos_navicella;
//pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

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
        "<OO",
        " \\/"};

char SpriteNemicoMorente[4][4]={
        " xx",
        "xxx",
        " xx"};

char SpriteNemicoAvanzato[4][4]={
        "< <",
        "   ",
        "< <"};

char SpriteNemicoAvanzatoMorente[4][4]={
        "x x",
        "   ",
        "x x"};

char SpriteMissile='>';

char SpriteBomba='@';

char logo[4][4]={
        "123",
        "456",
        "789"
};


    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mtxBuffer = PTHREAD_MUTEX_INITIALIZER;

    int posLettura =0 , posScrittura =0;


    
    sem_t piene,libere;

void scriveNelBuffer(Position oggetto);

Position leggeDalBuffer();

int maxx, maxy;

Position buffer[MAX];

int valoreDifficolta=1;



typedef struct {
    int x;
    int y;
    int idNemico;    
}valuesNemici;

typedef struct {
    int navx; 
    int navy;
    int diry;
}valuesMissili;

typedef struct {
    int x_bomba;
    int y_bomba;
    int id;
    pthread_t threaddino;
}valuesBomba;


int isMissileVivo1=0;
int isMissileVivo2=0;


pthread_t Tmissile1;
pthread_t Tmissile2;

int main() {
    int filedes[2], i, j=0, x_nemici, y_nemici, numColonne=1;
    pid_t pid_navicella, pid_nemici[numNemici];

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, 1); //funzione per leggere i tasti della tastiera
    curs_set(0);
    srand((int)time(NULL));

    getmaxyx(stdscr, maxy, maxx);
    x_nemici=maxx-5;
    y_nemici=3;

    refresh();
    nemiciVivi=numNemici;


    

    int spermino = 7;
    
    sem_init(&piene,0,0);
    sem_init(&libere,0,MAX);
    
    pthread_t Tnavicella;
    pthread_t Tcontrollo;
    //Creare questa roba in un loop con tante variabili per ogni nemico
    pthread_t Tnemico;

    pthread_create(&Tnavicella,NULL,navicella,NULL);
    //

    valuesNemici nemico1;

    nemico1.x = 30;
    nemico1.y = 10;
    nemico1.idNemico=0;  


    pthread_create(&Tnemico,NULL,nemiciPrimoLivello,(void *)&nemico1);
    //
    pthread_create(&Tcontrollo,NULL,controllo,NULL);
    

    pthread_join(Tnavicella, NULL);
    pthread_join(Tcontrollo, NULL);
    pthread_join(Tnemico, NULL);



    endwin();
    exit(0);
}



/**
 * Funzione che si occupa di generare le coordinate della navicella
 * @param pipeout File descriptor in scrittura della pipe
 * @param maxx Massimo valore delle X sullo schermo
 * @param maxy Massimo valore delle Y sullo schermo
 */
void* navicella(){
    //pthread_mutex_lock(&mtx);
    Position pos_navicella;
    pos_navicella.x=2;
    pos_navicella.y=maxy/2;
    //pos_navicella.y=6;
    pos_navicella.i=Navicella;
    //pthread_mutex_unlock(&mtx);
    //pid_t pid_missile1, pid_missile2;
    int isMissileVivo1=0, isMissileVivo2=0, sig1, sig2, c;

    scriveNelBuffer(pos_navicella);

    while(1) {

        if(isMissileVivo1==0 && isMissileVivo2==0){
                    pthread_join(Tmissile1, NULL);
                    pthread_join(Tmissile2, NULL);
        }
        c=getch();
        switch(c) {
            case KEY_UP:
                if (pos_navicella.y > 2) {
                    pos_navicella.y--;
                }
                break;
            case KEY_DOWN:
                if (pos_navicella.y < maxy - 3) {
                    pos_navicella.y++;
                }

                break;
            case ' ':
                //if(waitpid(pid_missile1,&sig1,WNOHANG)==pid_missile1){mvprintw(10,10,"SMEGMA");}
                // if(waitpid(pid_missile2,&sig2,WNOHANG)==pid_missile2){mvprintw(14,14,"BORRA");}
                
                //pthread_mutex_lock(&mtx);
                if (isMissileVivo1==0 && isMissileVivo2==0){
                    valuesMissili msl1, msl2;

                    msl1.navx = msl2.navx = pos_navicella.x;
                    msl1.navy = msl2.navy = pos_navicella.y;
                    msl1.diry = PASSO;
                    msl2.diry = -PASSO;
                    pthread_create(&Tmissile1,NULL,missile,(void *)&msl1);
                    pthread_create(&Tmissile2,NULL,missile,(void *)&msl2);

                } else //pthread_mutex_unlock(&mtx);
                break;
        }
        scriveNelBuffer(pos_navicella);
    }
}

/**
 * Funzione che si occupa di generare le coordinate di una navicella nemica
 * @param pipeout File descriptor in scrittura della pipe
 * @param x Coordinata X iniziale del nemico
 * @param y Coordinata Y iniziale del nemico
 * @param idNemico Numero identificativo della navicella nemica
 * @param maxx Massimo valore delle X sullo schermo
 * @param maxy Massimo valore delle Y sullo schermo
 */

void* nemiciPrimoLivello(void *arg){
    valuesNemici tmp = *(valuesNemici *)arg;
    int x = tmp.x;
    int y = tmp.y;
    int idNemico = tmp.idNemico;

    Position pos_nemico;
    pos_nemico.x=x;
    pos_nemico.y=y;
    pos_nemico.i=Nemico;
    pos_nemico.id=idNemico;
    //pos_nemico.pid=getpid();
    //pid_t pid_bomba;
    int r=1, dirx, diry, cicli=1;

    scriveNelBuffer(pos_nemico);
    while(1){
        dirx=-PASSO;
        pos_nemico.x+=dirx;

        if(r%2==0)
            diry=PASSO;
        else
            diry=-PASSO;
        if(pos_nemico.y+diry<2 || pos_nemico.y+diry>=maxy)
            diry=-diry;
        pos_nemico.y+=diry;
        r++;


        if(!(cicli++%5)){
            /*pid_bomba=fork();
            switch(pid_bomba){
                case -1:
                    perror("Errore nell'esecuzione della fork!");
                    exit(1);
                case 0:
                    bomba(pipeout, pos_nemico.x-1, pos_nemico.y+1, idNemico);
                default:
                    break;
            }*/

            pthread_t Tbombacurrnt;

            pos_nemico.Tthreadtokill=Tbombacurrnt;

            valuesBomba vb;

            vb.x_bomba=pos_nemico.x-1;
            vb.y_bomba=pos_nemico.y+1;
            vb.id=idNemico;
            vb.threaddino=Tbombacurrnt;
            pthread_create(&Tbombacurrnt,NULL,bomba,(void *)&vb);
            pthread_join(Tbombacurrnt, NULL);

        }
        scriveNelBuffer(pos_nemico);
        usleep(1200000);
    }
}


/**
 * Funzione che si occupa della stampa dei vari elementi a schermo e delle collisioni
 * @param pipein File descriptor in lettura della pipe
 * @param maxx Massimo valore delle X sullo schermo
 * @param maxy Massimo valore delle Y sullo schermo
 */
void* controllo(){

    //dichiarazione delle variabili dove verranno salvate le informazioni lette dalla pipe.
    Position nemico[numNemici], bombe[numNemici], bombeAvanzate[numNemici], navicella, valore_letto, missili[2];
    int i, punti=0, cicli=1, vite=3, n, j;
    //array che serve per tenere traccia dello stato deli nemici, ovvero se sono di primo (valore 0) o di secondo (valore 1) livello.
    int statoNemico[numNemici];
    //matrice per tenere traccia quali nemici di secondo livello sono vivi.
    int vitaNemici[numNemici][4];
    //inizializzazione delle variabili dichiarate sopra.
    navicella.x=-1;
    for(i=0; i<numNemici; i++){
        statoNemico[i]=0;
        nemico[i].x=-1;
        bombe[i].x=-1;
        bombeAvanzate[i].x=-1;
        for(j=0;j<4;j++){
            vitaNemici[i][j]=1;
        }
    }
    int nemiciVivi=numNemici, nemiciSecLiv=0;
    //stampa delle informazioni iniziali
    mvprintw(0, 1, "Vite: %d", vite);
    mvprintw(0, 10, "Nemici: %d  ", nemiciVivi);
    mvprintw(0, 40, "Nemici secondo livello: %d  ", nemiciSecLiv);
    mvprintw(0, maxx-15, "Punti: %d   ", punti);
    for(i=0; i<maxx; i++){
        mvprintw(1, i, "-");
    }
    do{
        //leggo un valore dalla pipe.
        valore_letto = leggeDalBuffer();
        /*pthread_mutex_lock(&mtx);
        valore_letto = pos_navicella;
        pthread_mutex_unlock(&mtx);
        */
        //mvprintw(2,1,"valore letto i: %d",valore_letto.i);   
        //controllo che tipo di valore ho letto.
        switch (valore_letto.i) {
            case Nemico:
                //cambio del colore per la stampa del nemico.
                attron(COLOR_PAIR(2));
                //controllo se il nemico letto dalla pipe è di primo o di secondo livello
                switch(statoNemico[valore_letto.id]){
                    case 0: //nemico di primo livello.
                        //elimino il nemico dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, "    ");
                        }
                        //aggiorno le coordinate del nemico.
                        nemico[valore_letto.id] = valore_letto;
                        //stampo il nemico.
                        for (i = 0; i < 3; i++) {
                            mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, SpriteNemicoBase[i]);
                        }
                        break;
                    //nemico di secondo livello.
                    case 1:
                        //elimino la stampa delle 4 navicelle (di quelle che sono ancora vive) dalle coordinate vecchie
                        if(vitaNemici[valore_letto.id][0]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x, "   ");
                            }
                        }
                        if(vitaNemici[valore_letto.id][1]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x + 4, "   ");
                            }
                        }
                        if(vitaNemici[valore_letto.id][2]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x, "   ");
                            }
                        }
                        if(vitaNemici[valore_letto.id][3]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x + 4, "   ");
                            }
                        }

                        mvprintw(nemico[valore_letto.id].y+3, nemico[valore_letto.id].x+3, " ");
                        //aggiorno le coordinate del nemico.
                        nemico[valore_letto.id] = valore_letto;
                        mvprintw(nemico[valore_letto.id].y+3, nemico[valore_letto.id].x+3, "X");

                        //stampo il nemico.
                        if(vitaNemici[valore_letto.id][0]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }
                        if(vitaNemici[valore_letto.id][1]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i, nemico[valore_letto.id].x + 4,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }
                        if(vitaNemici[valore_letto.id][2]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }
                        if(vitaNemici[valore_letto.id][3]==1) {
                            for (i = 0; i < 3; i++) {
                                mvprintw(nemico[valore_letto.id].y + i + 4, nemico[valore_letto.id].x + 4,
                                         SpriteNemicoAvanzato[i]);
                            }
                        }
                        break;
                }
                //imposto il colore di base.
                attron(COLOR_PAIR(1));
                //controllo se il nemico letto ha raggiunto la navicella (condizione per perdere).
                if(nemico[valore_letto.id].x<2){
                    //imposto le vite della navicella a zero.
                    vite=0;
                }
                break;
            case Navicella:
                //cambio del colore per la stampa della navicella.
                attron(COLOR_PAIR(3));
                //in base alle vite rimesta della navicella stampo uno strite diverso (der dare un senso di danneggiamento della navicella).
                switch(vite){
                    case 3:
                        //elimino la navicella dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella.
                        navicella = valore_letto;
                        //stampo la navicella.
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicella[i]);
                        }
                        break;
                    case 2:
                        //elimino la navicella dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella.
                        navicella = valore_letto;
                        //stampo la navicella.
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaDmgd2[i]);
                        }
                        break;
                    case 1:
                        //elimino la navicella dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella.
                        navicella = valore_letto;
                        //stampo la navicella.
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaDmgd3[i]);
                        }
                        break;
                    case 0:
                        //elimino la navicella dalle coordinate vecchie.
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        //aggiorno le coordinate della navicella.
                        navicella = valore_letto;
                        //stampo la navicella.
                        for(i=0; i<3; i++){
                            mvprintw(navicella.y+i, navicella.x, SpriteNavicellaMorta[i]);
                        }
                        break;
                }
                //imposto il colore di base.
                attron(COLOR_PAIR(1));
                break;

            case Missile:
                //elimino il missile dalle coordinate vecchie.
                mvaddch(missili[valore_letto.id].y, missili[valore_letto.id].x,' ');
                //aggiorno le coordinate del missile.
                missili[valore_letto.id] = valore_letto;
                //stampo il missile nella nuova posizione.
                mvaddch(missili[valore_letto.id].y, missili[valore_letto.id].x,SpriteMissile);
                //controllo delle collisioni del missile
                for(i=0; i<numNemici; i++){
                    for(n=0; n<2; n++){
                        //collisione missie-bomba.
                        if(bombe[i].x == missili[n].x && bombe[i].y == missili[n].y){
                            //elimino dallo schermo il missile e la bomba.
                            mvaddch(missili[n].y, missili[n].x, ' ');
                            //termino il processo che gestiva il missile.
                            

                            //kill(missili[n].pid, 1);

                            if(n==0){
                                mvprintw(1,2,"bestemmia");
                            int pthread_cancel(pthread_t Tmissile1);
                            } else {
                                mvprintw(1,2,"bestemmia");
                               int pthread_cancel(pthread_t Tmissile2); 
                            }
                            //termino il processo che gestiva la bomba.
                            

                            //kill(bombe[i].pid, 1);
                            

                            //imposto le coordnate del missile e della bomba fuori dallo schermo.
                            missili[n].x = -1;
                            missili[n].y = -1;
                            bombe[i].x = -1;
                            bombe[i].y = -1;
                        }
                        //nel caso ci siano delle bombe generate da navicelle di secondo livello controllo anche quelle collisioni.
                        if(statoNemico[i]==1){
                            if(bombeAvanzate[i].x == missili[n].x && bombeAvanzate[i].y == missili[n].y){
                                //elimino dallo schermo il missile e la bomba.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                

                                if(n==0){
                                int pthread_cancel(pthread_t Tmissile1);
                                } else {
                                   int pthread_cancel(pthread_t Tmissile2); 
                                }
                                

                                //termino il processo che gestiva la bomba.
                                

                                //kill(bombeAvanzate[i].pid, 1);
                                //imposto le coordnate del missile e della bomba fuori dallo schermo.
                                

                                missili[n].x = -1;
                                missili[n].y = -1;
                                bombeAvanzate[i].x = -1;
                                bombeAvanzate[i].y = -1;
                            }
                        }
                        //collisione missile-nemico di primo livello.
                        if(statoNemico[i]==0) {
                            if ((nemico[i].x == missili[n].x && nemico[i].y == missili[n].y) ||
                                (nemico[i].x + 1 == missili[n].x && nemico[i].y == missili[n].y) ||
                                (nemico[i].x + 2 == missili[n].x && nemico[i].y == missili[n].y)
                                || (nemico[i].x == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                (nemico[i].x + 1 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                (nemico[i].x + 2 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                                || (nemico[i].x == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                (nemico[i].x + 1 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                (nemico[i].x + 2 == missili[n].x && nemico[i].y + 2 == missili[n].y)) {
                                //aggiorno il numero di nemici presenti a schermo.
                                nemiciVivi+=3;
                                nemiciSecLiv+=4;
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                

                                if(n==0){
                                    mvprintw(1,2,"bestemmia");
                                    int pthread_cancel(pthread_t Tmissile1);
                                    refresh();
                                    usleep(20000);
                                } else {
                                    mvprintw(1,2,"bestemmia");
                                    int pthread_cancel(pthread_t Tmissile2);
                                    refresh();
                                    usleep(20000);
                                }

                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -1;
                                missili[n].y = -1;
                                //imposto il colore per la stampa del nemico.
                                attron(COLOR_PAIR(2));
                                //stampa dello sprite del nemico morente.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x, SpriteNemicoMorente[j]);
                                }
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=200;
                                        break;
                                    case 1:
                                        punti+=400;
                                        break;
                                    case 2:
                                        punti+=500;
                                        break;
                                }
                                //aggiorno lo schermo.
                                refresh();
                                //imposto un delay per far vedere il nemico morente.
                                usleep(30000);
                                //imposto il colore di base.
                                attron(COLOR_PAIR(1));
                                //modifico lo stato del nemico colpito da primo a secondo livello.
                                statoNemico[i] = 1;
                            }
                        }
                        //collisione missile-nemico di secondo livello.
                        else if(statoNemico[i]==1){
                            //collisione missile-nemico di secondo livello in alto a sinistra.
                            if(vitaNemici[i][0]==1 && ((nemico[i].x == missili[n].x && nemico[i].y == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y == missili[n].y)
                                                       || (nemico[i].x == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                                                       || (nemico[i].x == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 2 == missili[n].y))){
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                

                                if(n==0){
                            int pthread_cancel(pthread_t Tmissile1);
                            } else {
                               int pthread_cancel(pthread_t Tmissile2); 
                            }
                                

                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -1;
                                missili[n].y = -1;
                                //elimino dallo schermo il nemico di secondo livello colpito.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x, "   ");
                                }
                                //imposto il nemico di secondo livello colpito come morto.
                                vitaNemici[i][0]=0;
                                //diminuisco il numero di nemici vivi.
                                nemiciVivi--;
                                nemiciSecLiv--;
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=70;
                                        break;
                                    case 1:
                                        punti+=150;
                                        break;
                                    case 2:
                                        punti+=250;
                                        break;
                                }
                            }
                            //collisione missile-nemico di secondo livello in alto a destra.
                            if(vitaNemici[i][1]==1 && ((nemico[i].x + 4 == missili[n].x && nemico[i].y == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y == missili[n].y)
                                                       || (nemico[i].x + 4== missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 1 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 1 == missili[n].y)
                                                       || (nemico[i].x + 4== missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 2 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 2 == missili[n].y))){
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                

                                if(n==0){
                            int pthread_cancel(pthread_t Tmissile1);
                            } else {
                               int pthread_cancel(pthread_t Tmissile2); 
                            }
                                

                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -1;
                                missili[n].y = -1;
                                //elimino dallo schermo il nemico di secondo livello colpito.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j, nemico[i].x + 4, "   ");
                                }
                                //imposto il nemico di secondo livello colpito come morto.
                                vitaNemici[i][1]=0;
                                //diminuisco il numero di nemici vivi.
                                nemiciVivi--;
                                nemiciSecLiv--;
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=70;
                                        break;
                                    case 1:
                                        punti+=150;
                                        break;
                                    case 2:
                                        punti+=250;
                                        break;
                                }
                            }
                            //collisione missile-nemico di secondo livello in basso a sinistra.
                            if(vitaNemici[i][2]==1 && ((nemico[i].x == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 4== missili[n].y)
                                                       || (nemico[i].x == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 5 == missili[n].y)
                                                       || (nemico[i].x == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                                                       (nemico[i].x + 1 == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                                                       (nemico[i].x + 2 == missili[n].x && nemico[i].y + 6 == missili[n].y))){
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                

                                if(n==0){
                            int pthread_cancel(pthread_t Tmissile1);
                            } else {
                               int pthread_cancel(pthread_t Tmissile2); 
                            }
                                

                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -1;
                                missili[n].y = -1;
                                //elimino dallo schermo il nemico di secondo livello colpito.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j + 4, nemico[i].x, "   ");
                                }
                                //imposto il nemico di secondo livello colpito come morto.
                                vitaNemici[i][2]=0;
                                //diminuisco il numero di nemici vivi.
                                nemiciVivi--;
                                nemiciSecLiv--;
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=70;
                                        break;
                                    case 1:
                                        punti+=150;
                                        break;
                                    case 2:
                                        punti+=250;
                                        break;
                                }
                            }
                            //collisione missile-nemico di secondo livello in basso a destra.
                            if(vitaNemici[i][3]==1 && ((nemico[i].x + 4== missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 4 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 4== missili[n].y)
                                                       || (nemico[i].x + 4== missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 5 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 5 == missili[n].y)
                                                       || (nemico[i].x + 4== missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                                                       (nemico[i].x + 5 == missili[n].x && nemico[i].y + 6 == missili[n].y) ||
                                                       (nemico[i].x + 6 == missili[n].x && nemico[i].y + 6 == missili[n].y))){
                                //elimino dallo schermo il missile.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //termino il processo che gestiva il missile.
                                

                                if(n==0){
                            int pthread_cancel(pthread_t Tmissile1);
                            } else {
                               int pthread_cancel(pthread_t Tmissile2); 
                            }
                                

                                //imposto le coordnate del missile dallo schermo.
                                missili[n].x = -1;
                                missili[n].y = -1;
                                //elimino dallo schermo il nemico di secondo livello colpito.
                                for (j = 0; j < 3; j++) {
                                    mvprintw(nemico[i].y + j + 4, nemico[i].x + 4, "   ");
                                }
                                //imposto il nemico di secondo livello colpito come morto.
                                vitaNemici[i][3]=0;
                                //diminuisco il numero di nemici vivi.
                                nemiciVivi--;
                                nemiciSecLiv--;
                                //aggiungo i punti dati dal nemico colpito in base alla difficoltà.
                                switch(valoreDifficolta){
                                    case 0:
                                        punti+=70;
                                        break;
                                    case 1:
                                        punti+=150;
                                        break;
                                    case 2:
                                        punti+=250;
                                        break;
                                }
                            }
                            //se il blocco di nemici di secondo livello gestiti da un processo viene distrutto.
                            if(vitaNemici[i][0]==0 && vitaNemici[i][1]==0 && vitaNemici[i][2]==0 && vitaNemici[i][3]==0){
                                mvprintw(nemico[i].y+3, nemico[i].x+3, " ");
                                //imposto le coordinate deli nemici fuori dallo schermo.
                                nemico[i].x=-10;
                                nemico[i].y=-10;

                                //termino il processo che gestiva i nemici.
                                

                                //kill(nemico[i].pid, 1);
                                

                                //aggiorno lo schermo.
                                refresh();
                                //modifico lo stato del nemico colpito da secondo livello a distrutto.
                                statoNemico[i]=3;
                            }
                        }
                    }
                }
                break;
            case Bomba:
                //cambio del colore per la stampa della bomba.
                attron(COLOR_PAIR(2));
                //cancello la bomba dalle coordinate vecchie.
                mvaddch(bombe[valore_letto.id].y, bombe[valore_letto.id].x,' ');
                //aggiorno le coordinate della bomba.
                bombe[valore_letto.id] = valore_letto;
                //stampo la bomba nella nuova posizione.
                mvaddch(bombe[valore_letto.id].y, bombe[valore_letto.id].x,SpriteBomba);
                //collisione bomba-navicella
                for(i=0; i<numNemici; i++){
                    if((navicella.x==bombe[i].x && navicella.y==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y==bombe[i].y)
                       || (navicella.x==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+3==bombe[i].x && navicella.y+1==bombe[i].y) || (navicella.x+4==bombe[i].x && navicella.y+1==bombe[i].y)
                       || (navicella.x==bombe[i].x && navicella.y+2==bombe[i].y) || (navicella.x+1==bombe[i].x && navicella.y+2==bombe[i].y) || (navicella.x+2==bombe[i].x && navicella.y+2==bombe[i].y)){
                        //la navicella quando colpita da una bomba perde una vita.
                        vite++;
                        //aggiorno le coordinate della bomba che ha toccato la navicella fuori dallo schermo.
                        bombe[i].x=-1;
                        bombe[i].y=-1;
                        //termino il processo che gestisce la bomba.
                    }
                }
                //in base alle vite rimaste ambio lo sprite della navicella.
                attron(COLOR_PAIR(3));
                switch(vite) {
                    case 2:
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd2[i]);
                        }
                        break;
                    case 1:
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, "     ");
                        }
                        for (i = 0; i < 3; i++) {
                            mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd3[i]);
                        }
                        break;
                }
                attron(COLOR_PAIR(1));
                break;
            /*case BombaAvanzata:
                if(statoNemico[valore_letto.id]==1){
                    //cambio del colore per la stampa della bomba.
                    attron(COLOR_PAIR(2));
                    //cancello la bomba dalle coordinate vecchie.
                    mvaddch(bombeAvanzate[valore_letto.id].y, bombeAvanzate[valore_letto.id].x,' ');
                    //aggiorno le coordinate della bomba.
                    bombeAvanzate[valore_letto.id] = valore_letto;
                    //stampo la bomba nella nuova posizione.
                    mvaddch(bombeAvanzate[valore_letto.id].y, bombeAvanzate[valore_letto.id].x,SpriteBomba);
                    //collisione bomba-navicella
                    for(i=0; i<numNemici; i++){
                        if((navicella.x==bombeAvanzate[i].x && navicella.y==bombeAvanzate[i].y) || (navicella.x+1==bombeAvanzate[i].x && navicella.y==bombeAvanzate[i].y) || (navicella.x+2==bombeAvanzate[i].x && navicella.y==bombeAvanzate[i].y)
                           || (navicella.x==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y) || (navicella.x+1==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y) || (navicella.x+2==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y) || (navicella.x+3==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y) || (navicella.x+4==bombeAvanzate[i].x && navicella.y+1==bombeAvanzate[i].y)
                           || (navicella.x==bombeAvanzate[i].x && navicella.y+2==bombeAvanzate[i].y) || (navicella.x+1==bombeAvanzate[i].x && navicella.y+2==bombeAvanzate[i].y) || (navicella.x+2==bombeAvanzate[i].x && navicella.y+2==bombeAvanzate[i].y)){
                            //la navicella qkill(uando colpita da una bomba perde una vita.
                            vite--;
                            //aggiorno le coordinate della bomba che ha toccato la navicella fuori dallo schermo.
                            bombeAvanzate[i].x=-1;
                            bombeAvanzate[i].y=-1;
                            //termino il processo che gestisce la bomba.
                            kill(bombeAvanzate[i].pid, 1);
                        }
                    }
                    //in base alle vite rimaste ambio lo sprite della navicella.
                    attron(COLOR_PAIR(3));
                    switch(vite) {
                        case 2:
                            for (i = 0; i < 3; i++) {
                                mvprintw(navicella.y + i, navicella.x, "     ");
                            }
                            for (i = 0; i < 3; i++) {
                                mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd2[i]);
                            }
                            break;
                        case 1:
                            for (i = 0; i < 3; i++) {
                                mvprintw(navicella.y + i, navicella.x, "     ");
                            }
                            for (i = 0; i < 3; i++) {
                                mvprintw(navicella.y + i, navicella.x, SpriteNavicellaDmgd3[i]);
                            }
                            break;
                    }
                    attron(COLOR_PAIR(1));
                }
                break;*/
        }
        //ogni tot cicli i punti vengono decrementati.
        if(!(cicli++%500)){
            if(punti>0){
                punti--;
            }
        }
        //stampa delle informazioni aggiornate.
        mvprintw(0, 1, "Vite: %d", vite);
        mvprintw(0, 10, "Nemici: %d  ", nemiciVivi);
        mvprintw(0, 40, "Nemici secondo livello: %d  ", nemiciSecLiv);
        mvprintw(0, maxx-15, "Punti: %d   ", punti);
        for(i=0; i<maxx; i++){
            mvprintw(1, i, "-");
        }
        //aggiornamento degli elementi a schermo.
        refresh();
    } while(vite>0 && nemiciVivi>0); //ciclo termina quando la navicella non ha più vite oppure quando tutti i nemici sono stati distrutti.

    //pulisco lo schermo.
    clear();
    int exit=0;
    //stampa di game over quando si perde.
    if(vite==0){
        attron(COLOR_PAIR(3));
        for(i=0; i<7; i++){
           // mvprintw(maxy/2-10+i, maxx/2-50, gameover[i]);
        }
        attron(COLOR_PAIR(1));
        while(exit!=32){
        mvprintw(maxy/2-2, maxx/2-15,"Perso Hai totalizzato %d punti", punti);
        mvprintw(maxy/2, maxx/2-15,"Premi spazio per uscire");
        timeout(100);
        exit=getch();
        refresh();
    }

    }
    //stampa di game over quando si vince.
    else if(nemiciVivi==0 && valoreDifficolta!=3){
        attron(COLOR_PAIR(3));
        for(i=0; i<7; i++){
            //mvprintw(maxy/2-10+i, maxx/2-50, youwon[i]);
        }
        attron(COLOR_PAIR(1));
        while(exit!=32){
        mvprintw(maxy/2-2, maxx/2-13,"Vinto Hai totalizzato %d punti", punti);
        mvprintw(maxy/2, maxx/2-15,"Premi spazio per uscire");
        timeout(100);
        exit=getch();
        refresh();
        }
    }

}



void *missile(void *arg){

    valuesMissili tmp = *(valuesMissili *)arg;
    int navx = tmp.navx;
    int navy = tmp.navy;
    int diry = tmp.diry;
    Position pos_missile;
    pos_missile.x=5+navx;
    pos_missile.y=1+navy;
    pos_missile.i=Missile;
    //pos_missile.pid=getpid();
    if(diry==1){
        pos_missile.id=0;
    }
    else{
        pos_missile.id=1;
    }
    int i=0;
    scriveNelBuffer(pos_missile);
    while(!(pos_missile.x>maxx)){
        if(pos_missile.y+diry>maxy || pos_missile.y+diry<2) {diry=-diry;}
        if(i%6==0){
            pos_missile.y+=diry;
        }
        pos_missile.x+=1;
        scriveNelBuffer(pos_missile);
        usleep(10000);
        i++;
    }

    int status=0;
    pthread_mutex_lock(&mtx);
    isMissileVivo1=0;
    isMissileVivo2=0;
    void pthread_exit(void *status);
    pthread_mutex_unlock(&mtx);
    //kill(getpid(),SIGKILL);
    //exit(1);
}


void *bomba(void *arg){

    valuesBomba tmp = *(valuesBomba *)arg;
    int x_bomba = tmp.x_bomba;
    int y_bomba = tmp.y_bomba;
    int id = tmp.id;
    pthread_t threaddino = tmp.threaddino;

    Position pos_bomba;
    pos_bomba.x=x_bomba;
    pos_bomba.y=y_bomba;
    pos_bomba.i=Bomba;
    pos_bomba.id=id;


    scriveNelBuffer(pos_bomba);

    while(!(pos_bomba.x<0)){
        pos_bomba.x-=1;
        scriveNelBuffer(pos_bomba);
        usleep(30000);
    }
    int pthread_cancel(pthread_t threaddino);  
}



//come la Write
void scriveNelBuffer(Position oggetto){
    sem_wait(&libere);
    pthread_mutex_lock(&mtx);
        buffer[posScrittura] = oggetto;
        posScrittura = (posScrittura + 1)%MAX;
    pthread_mutex_unlock(&mtx);
    sem_post(&piene);
}

//come la Read
Position leggeDalBuffer(){
    Position tmp;
    sem_wait(&piene);
    pthread_mutex_lock(&mtx);
        tmp = buffer[posLettura];
        posLettura = (posLettura + 1)%MAX;
    pthread_mutex_unlock(&mtx);
    sem_post(&libere);
    return tmp;
}
