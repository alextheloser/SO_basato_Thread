#include "gestioneOggetti.h"

/**
 * Funzione che si occupa di generare le coordinate della navicella.
 */
void* navicella(){

    Position pos_navicella;
    pos_navicella.x=2;
    pos_navicella.y=maxy/2;
    pos_navicella.i=Navicella;
    int c;
    pthread_t Tnavicella=pthread_self();
    pthread_t Tmissile1;
    pthread_t Tmissile2;
    scriveNelBuffer(pos_navicella);
    while(1) {
        timeout(100);
        c = getch();
        switch (c) {
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
                if (isMissileVivo1 == 0 && isMissileVivo2 == 0){
                    pthread_mutex_lock(&mtx);
                    isMissileVivo2 = 1;
                    isMissileVivo1 = 1;
                    valuesMissili msl1, msl2;
                    msl1.navx = msl2.navx = pos_navicella.x;
                    msl1.navy = msl2.navy = pos_navicella.y;
                    msl1.diry = PASSO;
                    msl2.diry = -PASSO;
                    pthread_mutex_unlock(&mtx);
                    pthread_create(&Tmissile1, NULL, missile, (void *) &msl1);
                    pthread_create(&Tmissile2, NULL, missile, (void *) &msl2);
                    msl1.Tmissile = Tmissile1;
                    msl2.Tmissile = Tmissile2;
                }
                break;
        }
        pthread_mutex_lock(&mtx);
        if (isMissileVivo1 == 1) {
            pthread_detach(Tmissile1);
        }
        if (isMissileVivo2 == 1) {
            pthread_detach(Tmissile2);
        }
        pthread_mutex_unlock(&mtx);

        scriveNelBuffer(pos_navicella);

        pthread_mutex_lock(&mtx);
        if(turnodimorireNavicella==Tnavicella){
            turnodimorireNavicella=Tnavicella;
            pthread_mutex_unlock(&mtx);
            break;
        }
        pthread_mutex_unlock(&mtx);
    }
    pthread_exit((void *)1);
}

/**
 *
 * @param arg
 * @return
 */
void *missile(void *arg){
    valuesMissili tmp = *(valuesMissili *)arg;
    int navx = tmp.navx;
    int navy = tmp.navy;
    int diry = tmp.diry;
    pthread_t threadCorrente = tmp.Tmissile;
    Position pos_missile;
    pos_missile.x=5+navx;
    pos_missile.y=1+navy;
    pos_missile.i=Missile;
    pos_missile.Tthreadtokill=threadCorrente;
    if(diry==1){
        pos_missile.id=0;
    }
    else{
        pos_missile.id=1;
    }
    int i=0;
    scriveNelBuffer(pos_missile);
    if(pos_missile.id==0){
        while(isMissileVivo1==1) {
            if (pos_missile.y + diry > maxy || pos_missile.y + diry < 2) { diry = -diry; }
            if (i % 6 == 0) {
                pos_missile.y += diry;
            }
            pos_missile.x += 1;
            scriveNelBuffer(pos_missile);
            usleep(10000);
            i++;

            pthread_mutex_lock(&mtx);
            if ((threadCorrente == turnodimorire) || (pos_missile.x>maxx)) {
                turnodimorire=-1;
                pthread_mutex_unlock(&mtx);
                break;
            }
            pthread_mutex_unlock(&mtx);
        }
        isMissileVivo1=0;
    }
    else{
        while(isMissileVivo2==1) {
            if (pos_missile.y + diry > maxy || pos_missile.y + diry < 2) { diry = -diry; }
            if (i % 6 == 0) {
                pos_missile.y += diry;
            }
            pos_missile.x += 1;
            scriveNelBuffer(pos_missile);
            usleep(10000);
            i++;

            pthread_mutex_lock(&mtx);
            if ((threadCorrente == turnodimorire) || (pos_missile.x>maxx)) {
                turnodimorire=-1;
                pthread_mutex_unlock(&mtx);
                break;
            }
            pthread_mutex_unlock(&mtx);
        }
        isMissileVivo2=0;
    }
    pthread_exit((void *)1);
}

/**
 *
 * @param arg
 * @return
 */
void* nemici(void *arg){
    valuesNemici tmp = *(valuesNemici *)arg;
    int x = tmp.x;
    int y = tmp.y;
    int idNemico = tmp.idNemico;
    pthread_t threadCorrente = tmp.Tnemico;

    Position pos_nemico;
    pos_nemico.x=x;
    pos_nemico.y=y;
    pos_nemico.i=Nemico;
    pos_nemico.id=idNemico;
    pos_nemico.Tthreadtokill = pthread_self();
    //pos_nemico.pid=getpid();
    //pid_t pid_bomba;
    int r=1, dirx, diry, cicli=1;
    pthread_t Tbombacurrnt1, Tbombacurrnt2;

    scriveNelBuffer(pos_nemico);
    pthread_mutex_lock(&mtx);

    while(idNemico!=turnodimorireNemici) {
        pthread_mutex_unlock(&mtx);
        dirx = -PASSO;
        pos_nemico.x += dirx;

        if (r % 2 == 0) { diry = PASSO;}
        else { diry = -PASSO; }
        if (pos_nemico.y + diry < 2 || pos_nemico.y + diry >= maxy) { diry = -diry; }
        pos_nemico.y += diry;
        r++;
        scriveNelBuffer(pos_nemico);
        usleep(1200000);

        if (!(cicli++ % 5)) {
            valuesBomba vb[2];
            vb[0].x_bomba = pos_nemico.x - 1;
            vb[0].y_bomba = pos_nemico.y + 1;
            vb[0].id = idNemico;
            vb[0].i = Bomba;
            vb[0].threaddino = Tbombacurrnt1;
            pthread_create(&Tbombacurrnt1, NULL, bomba, (void *) &vb[0]);
            vb[1].x_bomba = pos_nemico.x - 1;
            vb[1].y_bomba = pos_nemico.y + 5;
            vb[1].id = idNemico;
            vb[1].i = BombaAvanzata;
            vb[1].threaddino = Tbombacurrnt2;
            pthread_create(&Tbombacurrnt2, NULL, bomba, (void *) &vb[1]);
            /*
            pthread_join(Tbombacurrnt1, NULL);
            pthread_join(Tbombacurrnt2, NULL);
            */
            pthread_detach(Tbombacurrnt1);
            pthread_detach(Tbombacurrnt2);
        }
    }
    mvprintw(15+idNemico,10,"muore il nemico %d",idNemico);
    refresh();
    turnodimorireBomba=Tbombacurrnt1;
    usleep(1000);
    pthread_detach(Tbombacurrnt1);
    turnodimorireBomba=Tbombacurrnt2;
    usleep(1000);
    pthread_detach(Tbombacurrnt2);
    pthread_mutex_unlock(&mtx);
    pthread_mutex_lock(&mtx);
    turnodimorireNemici=-1;
    pthread_mutex_unlock(&mtx);
    pthread_exit((void *)1);
}

void *bomba(void *arg){

    valuesBomba tmp = *(valuesBomba *)arg;
    int x_bomba = tmp.x_bomba;
    int y_bomba = tmp.y_bomba;
    int id = tmp.id;
    identity i = tmp.i;
    pthread_t threaddino = tmp.threaddino;

    Position pos_bomba;
    pos_bomba.x=x_bomba;
    pos_bomba.y=y_bomba;
    pos_bomba.i=i;
    pos_bomba.id=id;
    pos_bomba.Tthreadtokill=threaddino;


    scriveNelBuffer(pos_bomba);
    pthread_mutex_lock(&mtx);
    if(pos_bomba.i==Bomba){
        while(!(pos_bomba.x<0) && rindondanzaTurnodiMorireBomba!=id){
            //mvprintw(10+id,20,"%d:  %d    ",pos_bomba.id, pos_bomba.x);
            //refresh();
            pthread_mutex_unlock(&mtx);
            pos_bomba.x-=1;
            scriveNelBuffer(pos_bomba);
            usleep(30000);
        }
        rindondanzaTurnodiMorireBomba=-1;
    }
    if(pos_bomba.i==BombaAvanzata){
        while(!(pos_bomba.x<0) && rindondanzaTurnodiMorireBombaAvanzata!=id){
            //mvprintw(10+id,20,"%d:  %d    ",pos_bomba.id, pos_bomba.x);
            //refresh();
            pthread_mutex_unlock(&mtx);
            pos_bomba.x-=1;
            scriveNelBuffer(pos_bomba);
            usleep(30000);
        }
        rindondanzaTurnodiMorireBombaAvanzata=-1;
    }
    pthread_mutex_unlock(&mtx);
    //mvprintw(3,1,"SOBO WU          ");
    refresh();
    usleep(1000);
    /*pos_bomba.x=-100;
    pos_bomba.y=-100;*/
    pthread_exit((void *)1);
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
    pthread_mutex_lock(&mtx);
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
    pthread_mutex_unlock(&mtx);
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
        pthread_mutex_lock(&mtx);
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
                        pthread_mutex_unlock(&mtx);
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
                    pthread_mutex_lock(&mtx);
                    vite=0;
                    pthread_mutex_unlock(&mtx);
                }
                pthread_mutex_unlock(&mtx);
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
                pthread_mutex_unlock(&mtx);
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

                            //termino il processo che gestiva il missile.
                            //kill(missili[n].pid, 1);
                            pthread_mutex_lock(&mtx);
                            turnodimorire=missili[n].Tthreadtokill;
                            rindondanzaTurnodiMorireBomba=bombe[i].id;
                            pthread_mutex_unlock(&mtx);
                            //termino il processo che gestiva la bomba.

                            mvaddch(missili[n].y, missili[n].x, ' ');
                            //kill(bombe[i].pid, 1);


                            //imposto le coordnate del missile e della bomba fuori dallo schermo.
                            missili[n].x = -1;
                            missili[n].y = -1;
                            bombe[i].x = -2;
                            bombe[i].y = -2;
                        }
                        //nel caso ci siano delle bombe generate da navicelle di secondo livello controllo anche quelle collisioni.
                        if(statoNemico[i]==1){
                            if(bombeAvanzate[i].x == missili[n].x && bombeAvanzate[i].y == missili[n].y){
                                //elimino dallo schermo il missile e la bomba.

                                //termino il processo che gestiva il missile.

                                pthread_mutex_lock(&mtx);
                                turnodimorire=missili[n].Tthreadtokill;
                                rindondanzaTurnodiMorireBombaAvanzata=bombeAvanzate[i].id;
                                pthread_mutex_unlock(&mtx);

                                //termino il processo che gestiva la bomba.

                                mvaddch(missili[n].y, missili[n].x, ' ');
                                //kill(bombeAvanzate[i].pid, 1);
                                //imposto le coordnate del missile e della bomba fuori dallo schermo.


                                missili[n].x = -1;
                                missili[n].y = -1;
                                bombeAvanzate[i].x = -2;
                                bombeAvanzate[i].y = -2;
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

                                pthread_mutex_lock(&mtx);
                                turnodimorire=missili[n].Tthreadtokill;
                                pthread_mutex_unlock(&mtx);

                                //imposto le coordnate del missile dallo schermo.
                                mvaddch(missili[n].y, missili[n].x, ' ');
                                missili[n].x = -1;
                                missili[n].y = -1;
                                refresh();
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

                                pthread_mutex_lock(&mtx);
                                turnodimorire=missili[n].Tthreadtokill;
                                pthread_mutex_unlock(&mtx);


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

                                pthread_mutex_lock(&mtx);
                                turnodimorire=missili[n].Tthreadtokill;
                                pthread_mutex_unlock(&mtx);


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

                                pthread_mutex_lock(&mtx);
                                turnodimorire=missili[n].Tthreadtokill;
                                pthread_mutex_unlock(&mtx);


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


                                pthread_mutex_lock(&mtx);
                                turnodimorire=missili[n].Tthreadtokill;
                                pthread_mutex_unlock(&mtx);


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
                                nemico[i].x=1000;
                                nemico[i].y=1000;

                                //termino il processo che gestiva i nemici.
                                pthread_mutex_lock(&mtx);
                                //pthread_cancel(nemico[i].Tthreadtokill);
                                turnodimorireNemici=nemico[i].id;
                                //mvprintw(10,30,"ciao: %d",turnodimorire);
                                pthread_mutex_unlock(&mtx);
                                //pthread_join(nemico[i].Tthreadtokill, NULL);
                                //aggiorno lo schermo.
                                refresh();
                                //modifico lo stato del nemico colpito da secondo livello a distrutto.
                                statoNemico[i]=3;
                            }
                        }
                    }
                }
                pthread_mutex_unlock(&mtx);
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

                        pthread_mutex_lock(&mtx);
                        rindondanzaTurnodiMorireBomba=bombe[i].id;
                        usleep(1000);
                        vite--;
                        mvprintw(1,1,"STO FACENDO UNA COSA %d!!!!!!!",bombe[i].id);
                        refresh();
                        //usleep(999999);
                        //aggiorno le coordinate della bomba che ha toccato la navicella fuori dallo schermo.
                        bombe[i].x=-4;
                        bombe[i].y=-4;
                        //termino il processo che gestisce la bomba
                        pthread_mutex_unlock(&mtx);
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
                pthread_mutex_unlock(&mtx);
                break;
            case BombaAvanzata:
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
                            pthread_mutex_lock(&mtx);
                            rindondanzaTurnodiMorireBombaAvanzata=bombeAvanzate[i].id;
                            usleep(1000);
                            vite--;
                            mvprintw(2,1,"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa %d!!!!!!!",bombe[i].id);
                            refresh();
                            //usleep(999999);
                            //aggiorno le coordinate della bomba che ha toccato la navicella fuori dallo schermo.
                            bombeAvanzate[i].x=-4;
                            bombeAvanzate[i].y=-4;
                            //termino il processo che gestisce la bomba.
                            pthread_mutex_unlock(&mtx);
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
                pthread_mutex_unlock(&mtx);
                break;
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
    }while(vite>0 && nemiciVivi>0); //ciclo termina quando la navicella non ha più vite oppure quando tutti i nemici sono stati distrutti.

    pthread_mutex_lock(&mtx);
    pthread_exit((void *)1);
    pthread_mutex_unlock(&mtx);

    //pulisco lo schermo.
    clear();
    int uscita=0;

    //stampa di game over quando si perde.
    if(vite<=0){
        /*for (i = 0; i < numNemici; i++) {
                pthread_mutex_lock(&mtx);
                //pthread_cancel(nemico[i].Tthreadtokill);
                turnodimorireBomba=bombe[i].Tthreadtokill;
                //mvprintw(10,30,"ciao: %d",turnodimorire);
                pthread_mutex_unlock(&mtx);
                // mvprintw(10,10,"uccido il nemico %d", nemico[i].id);
    }
    for (i = 0; i < numNemici; i++) {
        pthread_mutex_lock(&mtx);
        //pthread_cancel(nemico[i].Tthreadtokill);
        turnodimorireBomba=bombeAvanzate[i].Tthreadtokill;
        //mvprintw(10,30,"ciao: %d",turnodimorire);
        pthread_mutex_unlock(&mtx);
        // mvprintw(10,10,"uccido il nemico %d", nemico[i].id);
    }*/

        for (i = 0; i < numNemici; i++) {
            if (statoNemico[i] == 0 || statoNemico[i] == 1) {
                pthread_mutex_lock(&mtx);
                //pthread_cancel(nemico[i].Tthreadtokill);
                turnodimorireNemici = nemico[i].id;
                //mvprintw(10,30,"ciao: %d",turnodimorire);
                pthread_mutex_unlock(&mtx);
                // mvprintw(10,10,"uccido il nemico %d", nemico[i].id);

                //usleep(1000);
            }
        }

        //pthread_mutex_unlock(&mtx);
        attron(COLOR_PAIR(3));
        for(i=0; i<7; i++){
            mvprintw(maxy/2-10+i, maxx/2-50, gameover[i]);
        }
        attron(COLOR_PAIR(1));
        while(uscita!=32){
            mvprintw(maxy/2-2, maxx/2-13,"Hai totalizzato %d punti", punti);
            mvprintw(maxy/2, maxx/2-11,"Premi spazio per uscire");
            timeout(10000);
            uscita=getch();
            refresh();
        }
        mvprintw(11,11,"porca madonnayyyyy");
        refresh();
        usleep(30000);

    }

    mvprintw(12,12,"porca sant'avendrace");
    refresh();
    //stampa di game over quando si vince.
    //pthread_mutex_lock(&mtx);
    if(nemiciVivi==0 && valoreDifficolta!=3 && vite>0){
        //pthread_mutex_unlock(&mtx);
        attron(COLOR_PAIR(3));
        for(i=0; i<7; i++){
            mvprintw(maxy/2-10+i, maxx/2-50, youwon[i]);
        }
        attron(COLOR_PAIR(1));
        while(uscita!=32){
            mvprintw(maxy/2-2, maxx/2-13,"Hai totalizzato %d punti", punti);
            mvprintw(maxy/2, maxx/2-11,"Premi spazio per uscire");
            timeout(1000);
            uscita=getch();
            refresh();
        }
    }
    mvprintw(13,13,"porco sant'efisio");
    refresh();
}

int menu(int maxx, int maxy){

    //nel caso in cui lo schermo sia troppo piccolo viene visualizzato questo messaggio
    if(maxx<140 || maxy<20){
        while(1){
            mvprintw(1,1,"Risoluzione troppo bassa");
            mvprintw(2,1,"Risoluzione minima: 140(x) caratteri per 20(y) caratteri");
            mvprintw(3,1,"Ridimensiona e riesegui :(");
            refresh();
        }
    }
    int isAnimationDone=0, scelta, numScelta=0, i, j=0;
    while(1){
        //stampa della linea superiore
        attron(COLOR_PAIR(1));
        mvprintw(0, 1, "SpaceDefenders");
        mvprintw(0, (maxx/2)-33, "                  Andrea Martis / Alessio Largiu      Unica 2021");

        for(i=0; i<maxx; i++){
            mvprintw(1, i, "-");
        }

        //stampa del logo
        if(isAnimationDone==0){

            usleep(10000);
            for(i=8; i<15; i++){
                attron(COLOR_PAIR(2));
                for (j = 0; j < 8; j++) {
                    mvprintw(i+j, (maxx/2)-63, logo[j]);
                }
                for (j = 0; j < 8; j++) {
                    mvprintw(i+j-6,  (maxx/2)-63, "                                                                                                                               ");
                }
                usleep(80000);
                refresh();
            }
            attron(COLOR_PAIR(1));
        }

        attron(COLOR_PAIR(3));
        for (j = 0; j < 8; j++) {
            mvprintw(i+j-1,  (maxx/2)-63, logo[j]);
        }

        //stampa della prima banda orizzontale
        attron(COLOR_PAIR(2));
        for(i=0; i<maxx; i++){
            mvprintw(22, i, "+");
            mvprintw(23, i, "+");
            if(isAnimationDone==0){
                usleep(7000);
                refresh();
            }
        }
        attron(COLOR_PAIR(1));

        //stampa selezione della difficoltà
        if(isAnimationDone==1){
            mvprintw(25, (maxx/2)-16, "Seleziona la modalita' di gioco");
            mvprintw(27, (maxx/2)-4, "Facile");
            mvprintw(28, (maxx/2)-4, "Medio");
            mvprintw(29, (maxx/2)-4, "Difficile");
            mvprintw(30, (maxx/2)-4, "Esci");

            switch(scelta){
                case KEY_UP:
                    numScelta--;
                    if(numScelta<0){
                        numScelta=3;
                    }
                    break;

                case KEY_DOWN:
                    numScelta++;

                    if(numScelta>3){
                        numScelta=0;
                    }
                    break;

                case 10:
                    return numScelta;
                    break;
            }

            //stampa del cursore
            attron(COLOR_PAIR(3));
            //int miStoComplicandoLaVita=0;
            for(j=0; j<=3; j++){
                if(j==numScelta){
                    mvprintw(27+j, (maxx/2)-6, ">");
                } else {
                    mvprintw(27+j, (maxx/2)-6, " ");
                }
            }
            attron(COLOR_PAIR(1));
        }

        //stampa della seconda banda orizzontale
        attron(COLOR_PAIR(3));
        for(i=maxx; i>=0; i--){
            mvprintw(32, i, "+");
            mvprintw(33, i, "+");
            if(isAnimationDone==0){
                usleep(7000);
                refresh();
            }
        }
        attron(COLOR_PAIR(1));

        //animazione della navicella e del nemico
        if(isAnimationDone==0){
            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, 10, SpriteNavicella[i]);
            }

            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, 132, SpriteNemicoBase[i]);
            }

            for(i=0;i<=117;i++){
                mvprintw(5, 16+i, ">");
                usleep(20000);
                mvprintw(5, 16+i-1, " ");
                refresh();

            }

            mvprintw(5,133," ");

            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, 132, SpriteNemicoMorente[i]);
            }
            refresh();
            usleep(200000);
            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, 132, "    ");
            }
            refresh();
            for(j=0; j<(maxx/2);j++){
                for (i = 0; i < 3; i++) {
                    mvprintw(4 + i, j+10, SpriteNavicella[i]);
                }
                for (i = 0; i < 3; i++) {
                    mvprintw(4 + i, 2-j+10, "    ");
                }
                refresh();
                usleep(10000);
            }
        }

        if(isAnimationDone==1){
            for (i = 0; i < 3; i++) {
                mvprintw(4 + i, maxx/2+10, SpriteNavicella[i]);
            }

        }

        timeout(100);
        scelta=getch();
        usleep(10000);
        refresh();
        isAnimationDone=1;
    }
}