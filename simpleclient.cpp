/* 
    File: simpleclient.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2013/01/31

    Simple client main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <unistd.h>

#include "reqchannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

// Index in request/stats threads represents people:
//     index 0 - Joe Smith
//     index 1 - Jane Smith
//     index 2 - John Doe
int num_request_threads = 3;
int request_counts[num_request_threads];
vector<int>[num_request_threads] histograms;

int num_requests = 10000;
int num_worker_threads = 15;
int buffer_size = 500;

BoundedBuffer* buffer;
BoundedBuffer[num_request_threads]* stats_buffer;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/
// Function to be performed by request thread
void* request_thread(void* req_id) {
    
}

// Function to be performed by worker thread
void* worker_thread(void* channel_id) {
    
}

// Function to be performed by stats thread
void* stats_thread(void* req_id) {
    
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    pthread_t request_threads[num_request_threads];
    pthread_t worker_threads[num_worker_threads];
    pthread_t stats_threads[num_request_threads];
    
    buffer = new BoundedBuffer(buffer_size);
    joe_buffer = new BoundedBuffer(buffer_size);
    
    int pid = fork();
    if (pid == 0) {
        //this process is the 'child', so run the dataserver
        system("./dataserver");
    } else {
        cout << "CLIENT STARTED:" << endl;

        cout << "Establishing control channel... " << flush;
        RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl;;

        cout << "Creating request threads...\n";
        for(int i = 0; i < num_request_threads; i++){
            pthread_create(&request_threads[i], NULL, request_thread, (void*)i);
        }
        
        cout << "Creating worker threads...\n";
        for(int i = 0; i < num_worker_threads; i++){
            string reply = chan.send_request("newthread");
            RequestChannel* channel = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);
            pthread_create(&worker_threads[i], NULL, worker_thread, channel);
        }
        
        cout << "Creating stats threads...\n";
        for(int i = 0; i < num_request_threads; i++){
            pthread_create(&stats_threads[i], NULL, stats_thread, (void*)i);
        }
        
        // Join threads
        for (int i = 0; i < num_request_threads; i ++)
            pthread_join(request_threads[i], NULL);
        
        cout << "Stopping worker and stats threads...\n";
        Response quit_response("kill", -1, -1);
        for(int i = 0; i < num_worker_threads; i++)
            buffer->push(quit_response);
        for(int i = 0; i < num_worker_threads; i++)
            pthread_join(worker_threads[i], NULL);
        for(int i = 0; i < num_request_threads; i++)
            pthread_join(stats_threads[i], NULL);
        
        string quit_reply = chan.send_request("quit");
        cout << "Reply to request 'quit' is '" << quit_reply << "'" << endl;
        sleep(1); // Waits until server fork is closed
        
        // Echo out statistics and histogram here
        cout << "Finished\n";
    }
}
