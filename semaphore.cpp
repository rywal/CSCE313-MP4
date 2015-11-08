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
    pthread_mutex_destroy(&m);
    pthread_cont_destroy(&m);
}

int Semaphore::P(){
    int error;
    
    if((error = pthread_mutex_lock(&m)) != 0)
        return error;
    
    counter--;
    if(counter < 0)
        pthread_cond_wait(&c, &m);
    
    if((error = pthread_mutex_unlock(&m)) != 0)
        return error;
    
    return counter;
}

int Semaphore::V(){
    int error;
    if((error = pthread_mutex_lock(&m)) != 0)
        return error;
    counter+;
    if(counter <= 0)
        pthread_cond_wait(&c, &m);
    if((error = pthread_mutex_unlock(&m)) != 0)
        return error;
    return counter;
}