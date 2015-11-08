#include "boundedbuffer.h"

using namespace std;

BoundedBuffer::BoundedBuffer() {
    
}

BoundedBuffer::BoundedBuffer(int _size){
    int size = _size;
    full = new Semaphore(0);
    empty = new Semaphore(size);
    mut = new Semaphore(1);
    
}

BoundedBuffer::~BoundedBuffer(){
    delete full;
    delete empty;
    delete mut;
    delete data;
}

void BoundedBuffer::push(Response item){
    empty->P(); // When this returns we know for a fact it is empty and ready for manipulation
    
    mut->P(); // Lock with semaphore of size 1, preventing others from using it
    data.push_back(item); // NOW we can safely change the data
    mut->v(); // Unlock to resume modifications
    
    full->V();
}

Response BoundedBuffer::pop(){
    full->P();
    
    mut->P(); // Lock with semaphore of size 1, preventing others from using it
    Response output = data.pop_back(); // NOW we can safely change the data
    mut->v(); // Unlock to resume modifications
    
    empty->V();
    return output;
}