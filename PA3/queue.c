#include <stdlib.h>
#include "queue.h"

int queue_init(queue* q, int maxSize, int n_files, int num_req, int num_res){
	int i;
	q->maxSize = maxSize;
	q->n_files = n_files;
	q->num_req = num_req;
	q->num_res = num_res;
	// Shared Array
	q->array = malloc(sizeof(queue_node) * (q->maxSize));
	if(!(q->array)){
		perror("Error: Could not allocate memory for shared array.");
		return 0;
	}

	for(i = 0; i < q->maxSize; i++){
		q->array[i].data = NULL;
	}
	
	q->head = 0;
	q->tail = 0;
	q->completed = 0;
	return q->maxSize;		
}

int isEmpty(queue* q){
	if((q->head == q->tail) &&(q->array[q->head].data == NULL)){
		return 1;
	}else{
		return 0;
	}
}

int isFull(queue* q){
	if((q->head == q->tail) && (q->array[q->head].data != NULL)){
		return 1;
	}else{	
		return 0;
	}
}

void* pop(queue* q){
	void* data;

	if(isEmpty(q)){
		return NULL;
	}
	data = q->array[q->head].data;
	q->array[q->head].data = NULL;
	q->head = (q->head + 1) % q->maxSize;

	return data;
}

int push(queue* q, void* data){
	if(isFull(q)){
		return 0;
	}
	q->array[q->tail].data = data;
	q->tail = (q->tail + 1) % q->maxSize;
	return 1;
}

void clean(queue* q){
	while(!isEmpty(q)){
		pop(q);
	}
	free(q->array);
}

