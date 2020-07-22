Author: John Furlong
Date: March 30th, 2020
Assignment 3: DNS Name Resolution Engine

Submission Files:

multi-lookup.c:
- Program takes a set of file names as input, each with a hostname for every line. Every file gets serviced by an individual thread from the requestor pool, which is then serviced by a set of resolver threads to map each hostname to its IP address.

multi-lookup.h:
- Function prototypes for multi-lookup.c

queue.h:
- Header file for queue.c -- standard FIFO queue struct

Makefile:
- GNU makefile to build the program -- <make clean> to reset

/input:
- Directory of input files

performance.txt:
- Performance diagnostic for varied combinations of the number of requester / resolver threads


Build Instructions:

1.	Run Make to compile all files

2.	Execute using the following format:
	
	" ./multi-lookup < # requesters >  < # resolvers >  < serviced.txt >  < results.txt >  < names1.txt > ...\

3. To check for memory leaks:
	
	" valgrind ./multi-lookup  names1.txt names2.txt ... namesN.txt results.txt 
