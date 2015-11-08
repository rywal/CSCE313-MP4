// BoundedBuffer
// boundedbuffer.h
// Created by: Ryan Walters
// Date Created: November 8, 2015

#ifndef _boundedbuffer_H_                   // include file only once
#define _boundedbuffer_H_

#include <string>
#include <vector>
#include "semaphore.h"

class BoundedBuffer {
    // We want bounded buffer to be overflow safe(doesnt surpass bound, b)
    // These should not be constructed here.. Need to do this in the constructor when coding
private:
    Semaphore full;
    Semaphore empty;
    Semaphore mut;
    
    int size;
    vector<string> data; // Main data structure that we want to make thread-safe
    
public:
    BoundedBuffer(int _size);
    ~BoundedBuffer();
    void push();
    string pop();
};

#endif