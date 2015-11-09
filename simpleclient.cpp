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
#include <stdint.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector>

#include <errno.h>
#include <unistd.h>

#include "reqchannel.h"
#include "boundedbuffer.h"

#define NUM_PEOPLE 3

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

// Index in request/stats threads represents people:
//     index 0 - Joe Smith
//     index 1 - Jane Smith
//     index 2 - John Doe
int num_request_threads = NUM_PEOPLE;
int request_counts[NUM_PEOPLE];

int num_requests = 10000;
int num_worker_threads = 15;
int buffer_size = 300;

BoundedBuffer *buffer;
BoundedBuffer *response_buffers[NUM_PEOPLE];

int histograms[NUM_PEOPLE][100]; // 100 possible numbers for each of the request threads
int* name_ids[NUM_PEOPLE];

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/
// Function to be performed by request thread
void* request_thread(void* req_id) {
//    printf("Request thread id is %i\n", req_id);
    int request_id = *((int*) req_id);
    const string request_names[] = {"Joe Smith", "Jane Smith", "John Doe"};
    
    for(int i = 0; i < num_requests; i++){
        Response res = Response("something", request_id, 0);
        request_counts[request_id]++;
        res.data = "data " + request_names[request_id];
        res.req_id = request_id;
        res.req_number = request_counts[request_id];
        buffer->push(res);
    }
    
    return 0;
}

// Function to be performed by worker thread
void* worker_thread(void* channel_id) {
    RequestChannel *channel = (RequestChannel *) channel_id;
    Response response("something", 0, 0);
    
    // Keep sending responses from buffer until told to 'quit'
    while(true){
        // Pull next response from buffer
        response = buffer->pop();
        //cout << "Response: " << response.data << "\n";
        // Quit if told to
        if(response.data == "quit") break;
    
        // Send request, and save response to appropriate buffer
        string reply = channel->send_request(response.data);
        response.data = reply;
        response_buffers[response.req_id]->push(response);
    }
    channel->send_request("quit");
    
    return 0;
}

// Function to be performed by stats thread
void* stats_thread(void* req_id) {
    cout << "Opened stats_thread for req_id: " << *((int*)req_id) << "\n";
    int request_id = *((int *)req_id);
    
    Response res("something", -1, -1);
    for(int i = 0; i < num_requests; i++){
        res = response_buffers[request_id]->pop();
        histograms[request_id][atoi(res.data.c_str())]+=1;
        cout << "popped a request" << request_id << ":" << i  << " Data: " << res.data << "\n";
    }
    
    cout << "Stats thread for request " << request_id << " finished\n";
    
    return 0;
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    pthread_t request_threads[NUM_PEOPLE];
    pthread_t worker_threads[num_worker_threads];
    pthread_t stats_threads[NUM_PEOPLE];
    
    buffer = new BoundedBuffer(buffer_size);
    for(int i = 0; i < num_request_threads; i ++)
        response_buffers[i] = new BoundedBuffer(buffer_size);
    
    for(int i = 0; i < NUM_PEOPLE; i++){
        name_ids[i] = new int(i);
    }
    
    int pid = fork();
    if (pid == 0) {
        //this process is the 'child', so run the dataserver
        system("./dataserver > /dev/null");
    } else {
        cout << "CLIENT STARTED:" << endl;

        cout << "Establishing control channel... " << flush;
        RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl;;

        cout << "Creating request threads...\n";
        for(int i = 0; i < num_request_threads; i++){
            pthread_create(&request_threads[i], NULL, request_thread, (void*)name_ids[i]);
        }
        
        // Create new channels prior to creating the worker threads to avoid deadlock issue
        RequestChannel* channels[num_worker_threads];
        for(int i=0; i < num_worker_threads; i++){
            string reply = chan.send_request("newthread");
            channels[i] = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);
        }
        
        cout << "Creating worker threads...\n";
        for(int i = 0; i < num_worker_threads; i++){
            pthread_create(&worker_threads[i], NULL, worker_thread, channels[i]);
        }
        
        cout << "Creating stats threads...\n";
        for(int i = 0; i < num_request_threads; i++){
            pthread_create(&stats_threads[i], NULL, stats_thread, (void*)name_ids[i]);
        }
        cout << "done\n";
        
        cout << "Joining request threads\n";
        // Join threads
        for (int i = 0; i < num_request_threads; ++i){
            pthread_join(request_threads[i], NULL);
        }
        cout << "done\n";
        
        cout << "Stopping worker and stats threads...\n";
        Response quit_response("quit", -1, -1);
        for(int i = 0; i < num_worker_threads; i++)
            buffer->push(quit_response);
        cout << "Added quits to buffer\n";
        
        for(int i = 0; i < num_worker_threads; i++)
            pthread_join(worker_threads[i], NULL);
        cout << "Joined worker threads\n";
        
        for(int i = 0; i < num_request_threads; i++)
            pthread_join(stats_threads[i], NULL);
        cout << "Joined stats threads\n";
        
        string quit_reply = chan.send_request("quit");
        cout << "Reply to request 'quit' is '" << quit_reply << "'" << endl;
        sleep(1); // Waits until server fork is closed
        
        // Echo out statistics and histogram here
        cout << "Finished\n";
    }
}
