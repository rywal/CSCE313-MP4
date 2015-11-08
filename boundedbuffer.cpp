#include "boundedbuffer.h"

BoundedBuffer::BoundedBuffer(int _size){
    int size = _size;
    full = Semaphore(0);
    empty = Semaphore(size);
    mut = Semaphore(1);
    
}

BoundedBuffer::~BoundedBuffer(){
    delete size;
    delete full;
    delete empty;
    delete mut;
    delete data;
}

void BoundedBuffer::push(string item){
    empty.P(); // When this returns we know for a fact it is empty and ready for manipulation
    
    mut.P(); // Lock with semaphore of size 1, preventing others from using it
    data.push_back(item); // NOW we can safely change the data
    mut.v(); // Unlock to resume modifications
    
    full.V();
}

string BoundedBuffer::pop(){
    full.P();
    
    mut.P(); // Lock with semaphore of size 1, preventing others from using it
    string output = data.pop_back(item); // NOW we can safely change the data
    mut.v(); // Unlock to resume modifications
    
    empty.V();
    return output;
}