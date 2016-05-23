# Data Server

Simple data server improves on Machine problem 3 to implement threads and a semaphore to allow concurrent connections to the data server. Semaphores control the bounded buffer between request threads and worker threads.

The client, `simpleclient`, is to be run using the following form:
```
simpleclient -n <number of data requests per person>
             -b <size of bounded buffer in requests>
             -w <number of worker threads>
```

Performance metrics were measured using varying numbers of worker threads and sizes of the buffer. The results are included in `report.pdf`.

_Created as part of CSCE 313 at Texas A&M University_

Contributors:
* Ryan Walters
* Garrett Haynes
