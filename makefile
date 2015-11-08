# makefile
# adding capability to use makefile in various environments without modifications
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
cc = clang++
else ifeq ($(UNAME), Darwin)
cc = g++
else
cc = g++-4.7
endif

all: dataserver simpleclient

reqchannel.o: reqchannel.h reqchannel.cpp
	$(cc) -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	$(cc) -g -o dataserver dataserver.cpp reqchannel.o -lpthread

simpleclient: simpleclient.cpp reqchannel.o
	$(cc) -g -o simpleclient simpleclient.cpp reqchannel.o
