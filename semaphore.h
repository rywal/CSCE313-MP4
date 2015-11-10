/* 
    File: semaphore.H

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/11

*/

#ifndef _semaphore_H_                   // include file only once
#define _semaphore_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <pthread.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   S e m a p h o r e  */
/*--------------------------------------------------------------------------*/

class Semaphore {
private:
    /* -- INTERNAL DATA STRUCTURES
     You may need to change them to fit your implementation. */

    int             value;
    pthread_mutex_t m;
    pthread_cond_t  c;

public:

    /* -- CONSTRUCTOR/DESTRUCTOR */
    Semaphore(int _val){
        value = _val;
        pthread_mutex_init(&m, NULL);
        pthread_cond_init(&c, NULL);
    }
    
    ~Semaphore(){
        pthread_mutex_destroy(&m);
        pthread_cond_destroy(&c);
    }

    /* -- SEMAPHORE OPERATIONS */

    int P(){
        int error;
        
        if((error = pthread_mutex_lock(&m)) != 0)
            return error;
        
        value--;
        if(value < 0){
            cout << "(\n";
            pthread_cond_wait(&c, &m);
            cout << ")\n";
        }
        
        if((error = pthread_mutex_unlock(&m)) != 0)
            return error;
        
        return value;
    }
    
    int V(){
        int error;
        if((error = pthread_mutex_lock(&m)) != 0)
            return error;
        value++;
        if(value <= 0)
            pthread_cond_signal(&c);
        if((error = pthread_mutex_unlock(&m)) != 0)
            return error;
        return value;
    }
};


#endif


