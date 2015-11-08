class BoundedBuffer {
    // We want bounded buffer to be overflow safe(doesnt surpass bound, b)
    // These should not be constructed here.. Need to do this in the constructor when coding
    Sema full(0);
    Sema empty(b);
    Sema mut(1);
    
    vector<string> data; // Main data structure that we want to make thread-safe
    void push();
    string pop();
};

// implementation of push function
void BB:push(string item){
    empty.P(); // When this returns we know for a fact it is empty and ready for manipulation
    
    //data.push_back(item); // This is still not thread-safe...( could have up to b threads modifying it)
    mut.P(); // Lock with semaphore of size 1, preventing others from using it
    data.push_back(item); // NOW we can safely change the data
    mut.v(); // Unlock to resume modifications
    
    full.V();
}

string BB:pop(){
    full.P();
    
    mut.P(); // Lock with semaphore of size 1, preventing others from using it
    data.pop_back(item); // NOW we can safely change the data
    mut.v(); // Unlock to resume modifications
    
    empty.V();
}