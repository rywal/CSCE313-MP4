//
//  semaphore.cpp
//  
//
//  Created by Ryan Walters on 11/8/15.
//
//

#include <stdio.h>
#include "semaphore.h"

Semaphore::Semaphore(int _val){
    counter = _val;
    pthread_mutex_init(&m);
    pthread_cond_init(&c);
}

Semaphore::~Semaphore(){
    delete counter;
    pthread_mutex_destroy(&m);
    pthread_cont_destroy(&m);
}

int Semaphore::P(){
    pthread_mutex_lock(&m);
    counter--;
    if(counter < 0)
        pthread_cond_wait(&c, &m);
    pthread_mutex_unlock(&m);
}

int Semaphore::V(){
    pthread_mutex_lock(&m);
    counter++;
    if(counter <= 0)
        pthread_cond_signal(&c);
    pthread_mutex_unlock(&m);
}