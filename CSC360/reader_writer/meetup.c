/*Required Headers*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 */
pthread_mutex_t groupArriving;
pthread_cond_t quota;
int groupSize;
int mfl;
int peoplePresent;
int groupNumber;
resource_t codeword;

void initialize_meetup(int n, int mf) {
    // n = number of bobafetts needed to make a group
    // mf = 0 or 1; 0 means meetlast, 1 means meetfirst

    char label[100];
    int groupSize = n;
    int mfl = mf;
    int peoplePresent = 0;
    int groupNumber = 0;

    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
    pthread_mutex_init(&groupArriving, NULL);
    pthread_cond_init(&quota, NULL);
}


void join_meetup(char *value, int len) {
    if (mfl == MEET_FIRST){
        pthread_mutex_lock(&groupArriving);
        peoplePresent++;
        if(peoplePresent <= groupSize){
            int thisGroupNUmber = groupNumber;
            while(thisGroupNUmber = groupNumber){
                if (peoplePresent == 1){
                    write_resource(&codeword, value, len);
                }
                pthread_cond_wait(&quota, &groupArriving);
            }
        }
        else{
            peoplePresent = 0;
            groupNumber++;
            pthread_cond_broadcast(&quota);
        }
        pthread_mutex_unlock(&groupArriving);
    }
    else{
        pthread_mutex_lock(&groupArriving);
        peoplePresent++;
        if(peoplePresent <= groupSize){
            int thisGroupNUmber = groupNumber;
            while(thisGroupNUmber = groupNumber){
                if (peoplePresent == groupSize){
                    write_resource(&codeword, value, len);
                }
                pthread_cond_wait(&quota, &groupArriving);
            }
        }
        else{
            peoplePresent = 0;
            groupNumber++;
            pthread_cond_broadcast(&quota);
        }
        pthread_mutex_unlock(&groupArriving);
    }
}