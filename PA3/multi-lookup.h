#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

#include<pthread.h>
#include<stdlib.h>
#include<errno.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/syscall.h>
#include<time.h>
#define gettid() syscall(SYS_gettid)

#include "queue.h"
#include "util.h"

// Parse input files
void* readFiles(char* filename);

// Create requester thread pool
void* requesterPool(char* inputFiles);

// Perform DNS lookup
void* dns(FILE* outputf);

// Create resolver thread pool
void* resolverPool(FILE* outputf);

int main(int argc, char* argv[]);

#endif




