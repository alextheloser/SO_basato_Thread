#include "gestioneBuffer.h"

/**
 * Funzione che si occupa di scrivere un oggetto nel buffer (analoga alla write).
 * @param oggetto Elemento da scrivere nel buffer.
 */
void scriveNelBuffer(Position oggetto){
    sem_wait(&libere);
    pthread_mutex_lock(&mtxBuffer);
    buffer[posScrittura] = oggetto;
    posScrittura = (posScrittura + 1)%MAX;
    pthread_mutex_unlock(&mtxBuffer);
    sem_post(&piene);
}

/**
 * Funzione che si occupa di leggere un oggetto nel buffer (analoga alla read).
 * @return Elemento letto dal buffer.
 */
Position leggeDalBuffer(){
    Position tmp;
    sem_wait(&piene);
    pthread_mutex_lock(&mtxBuffer);
    tmp = buffer[posLettura];
    posLettura = (posLettura + 1)%MAX;
    pthread_mutex_unlock(&mtxBuffer);
    sem_post(&libere);
    return tmp;
}