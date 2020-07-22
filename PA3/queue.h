#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include "multi-lookup.h"

typedef struct q_node{
	void* data;
} queue_node;

typedef struct Queue{
	queue_node* array;
	int head;
	int tail;
	int maxSize;
	int completed;
	int n_files;
	int num_req;
	int num_res;
} queue;

// Initialize a new queue.
// Failure = 0 | Success = 1
int queue_init(queue* q, int size, int n_files, int num_req, int num_res);

// Queue Empty: Failure = 0 | Success = 1
int isEmpty(queue* q);

// Queue Full: Failure = 0 | Success = 1
int isFull(queue* q);

// FIFO Queue Push: Failure = 0 | Success = 1
int push(queue* q, void* data);

// FIFO Queue Pop: Failure = 0 | Success = 1
void* pop(queue* q);

// Destructor to avoid memory leaks
void clean(queue* q);

#endif


