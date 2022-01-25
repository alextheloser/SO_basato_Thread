#include "gestioneBuffer.h"

/**
 * Funzione che si occupa di scrivere un oggetto nel buffer (analoga alla write).
 * @param oggetto Elemento da scrivere nel buffer.
 */
void scriveNelBuffer(Position oggetto){
    //se il buffer è pieno si blocca e riprende quando si libera uno spazio.
    sem_wait(&libere);
    pthread_mutex_lock(&mtxBuffer);
    //scrivo nel buffer l'oggetto.
    buffer[posScrittura] = oggetto;
    posScrittura = (posScrittura + 1)%MAX;
    pthread_mutex_unlock(&mtxBuffer);
    //aumento il numero di oggetti presenti nel buffer.
    sem_post(&piene);
}

/**
 * Funzione che si occupa di leggere un oggetto nel buffer (analoga alla read).
 * @return Elemento letto dal buffer.
 */
Position leggeDalBuffer(){
    Position tmp;
    //se il buffer è vuoto si blocca e riprende quando viene inserito un oggetto.
    sem_wait(&piene);
    pthread_mutex_lock(&mtxBuffer);
    //lettura dell'oggetto dal buffer.
    tmp = buffer[posLettura];
    posLettura = (posLettura + 1)%MAX;
    pthread_mutex_unlock(&mtxBuffer);
    //aumento il numero di posti liberi nel buffer
    sem_post(&libere);
    //restituisco l'oggetto letto
    return tmp;
}