#include "nemici.h"

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
