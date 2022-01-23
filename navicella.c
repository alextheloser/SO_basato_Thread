#include "navicella.h"

/**
 * Funzione che si occupa di generare le coordinate della navicella.
 */
void* navicella(){
    Position pos_navicella;
    pos_navicella.x=2;
    pos_navicella.y=maxy/2;
    pos_navicella.i=Navicella;
    int c;
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
