/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "rw.h"
#include "resource.h"

/*
 * Declarations for reader-writer shared variables -- plus concurrency-control
 * variables -- must START here.
 */
sem_t reader;
sem_t writer;
int readercount;
int writercount;
static resource_t data;

void initialize_readers_writer() {
    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
    sem_init(&reader, 0 ,0);
    sem_init(&writer, 0 ,1);
    readercount = 0;
    writercount = 0;
}


void rw_read(char *value, int len) {
    sem_wait(&reader);
    if(readercount != 0){
        sem_wait(&writer);
        read_resource(&data, value, len);
    }
    sem_post(&reader);

    sem_wait(&reader);
    readercount--;
    if(readercount==0){
        sem_post(&writer);
    }
    sem_post(&reader);
}


void rw_write(char *value, int len) {
    sem_wait(&writer);
    if(readercount == 0){
        sem_wait(&reader);
        write_resource(&data, value, len);
    }
    sem_post(&writer);

    sem_wait(&writer);
    if(readercount != 0){
        sem_post(&reader);
    }
    sem_post(&writer);
}