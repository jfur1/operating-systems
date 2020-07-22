#include "multi-lookup.h"

// Condition Variables
pthread_cond_t arr_full;
pthread_cond_t arr_empty;

// Mutex Locks
pthread_mutex_t arr_lock;
pthread_mutex_t req_lock;
pthread_mutex_t res_lock;

// Instance of Structs: Shared Array & Output Files
queue q;
FILE* serviced;
FILE* outputf;

void* readFiles(char* filename){
	fprintf(serviced, "Requestor Thread %lu serviced file: %s\n", gettid(), filename);
	char path[MAX_NAME_LENGTH] = "input/";
	strcat(path, filename);

	// Open the input file
	FILE* input_file = fopen(path, "r");

	// Error Handling: Bogus File Path
	if(!input_file){
		perror("Error: could not open file.\n");
		// Lock the counter to avoid infinite loops
		pthread_mutex_lock(&req_lock);
		q.completed++;
		pthread_mutex_unlock(&req_lock);
		return NULL;
	}
	char hostname[MAX_NAME_LENGTH];

	while(fscanf(input_file, "%1024s", hostname) > 0){
		// Check and see if array is full; protect access via mutex lock
		pthread_mutex_lock(&arr_lock);
		while(isFull(&q)){
			// If array is full, thread waits on arr_full condition.
			pthread_cond_wait(&arr_full, &arr_lock);
		}
		// Else, push hostname into the queue
		push(&q, strdup(hostname));
		// signal queueEmpty condition to notify that something's in the array
		pthread_cond_signal(&arr_empty);
		// Unlock access to the array
		pthread_mutex_unlock(&arr_lock);
	}
	// Lock access to counter so we can increment # of files added to the shared array
	pthread_mutex_lock(&req_lock);
	
	q.completed++;

	// Unlock access to counter
	pthread_mutex_unlock(&req_lock);
	
	fclose(input_file);
	return NULL;
}

void* requesterPool(char* Files){

	// Declare a ptr to an array of strings, to avoid warning.
	char** input_filenames = (char**) Files;
	// Initialize requestor threads
	pthread_t req_threads[q.n_files];
	// Create requestor threads for each input file
	for(int i = 0; i < q.n_files; i++){
		char* file_name = input_filenames[i];
		pthread_create(&req_threads[i], NULL, (void*) readFiles, (void*) file_name);
		// Wait to proceed until req_threads have finished executing
		pthread_join(req_threads[i], NULL); 
	}
	for(int i = 0; i < q.n_files; i++){
		// Signal requestor threads that queue empty
		pthread_cond_signal(&arr_empty);
	}
	return NULL;
}

void* dns(FILE* output_f){	
	while(1){
		// Lock array mutex to access
		pthread_mutex_lock(&arr_lock);
		// Check to see if the array is empty
		while(isEmpty(&q)){
			if(q.completed == q.n_files){
				pthread_mutex_unlock(&arr_lock);
				return NULL;
			}
			// If array is empty but files still remain, then consumer threads must wait on arr_empty condition
			pthread_cond_wait(&arr_empty, &arr_lock);
		}

		char* hostname = (char*) pop(&q);

		// Signal producers that there is more space
		pthread_cond_signal(&arr_full);
		// allocate IP address
		char first_ip[MAX_IP_LENGTH];
		// Error Handling 1: Bogus Hostname
		if(dnslookup(hostname, first_ip, sizeof(first_ip)) == UTIL_FAILURE){
			fprintf(stderr, "Error: DNS failed to lookup hostname: %s\n", hostname);
			strncpy(first_ip, "", sizeof(first_ip));
		}
		// Lock the output file using res_lock
		pthread_mutex_lock(&res_lock);
		// Insert the hostname with its IP address
		fprintf(output_f, "%s,%s\n", hostname, first_ip);
		// Unlock resolver mutex to access output file
		pthread_mutex_unlock(&res_lock);
		// free space for the next hostname
		free(hostname);
		// Unlock array access
		pthread_mutex_unlock(&arr_lock);
	}
	return NULL;
}

void* resolverPool(FILE* outputf){
	// Initialize resolver threads
	pthread_t res_threads[q.n_files];
	int i;
	for(i = 0; i < q.n_files; i++){
		pthread_create(&res_threads[i], NULL, (void *) dns, outputf);
		// Wait to proceed until res_threads have finished
		pthread_join(res_threads[i], NULL);
	}
	return NULL;
}

int main(int argc, char* argv[]){
	// Start Timer
	struct timeval start, end;
	gettimeofday(&start, NULL);

	int num_req = atoi(argv[1]);
	int num_res = atoi(argv[2]);

	// Initialized shared array
	queue_init(&q, 50, argc-5, num_req, num_res);

	// Initialize condition vars
	pthread_cond_init(&arr_full, NULL);
	pthread_cond_init(&arr_empty, NULL);
	// Initialize Mutex locks
	pthread_mutex_init(&arr_lock, NULL);
	pthread_mutex_init(&req_lock, NULL);
	pthread_mutex_init(&res_lock, NULL);

	// Array of Input File Names	
	char* inputFiles[q.n_files];
	for(int i = 0; i< q.n_files; i++){
		inputFiles[i] = argv[i+5];
	}
	
	// Open output files
	serviced = fopen("serviced.txt", "w");
	outputf = fopen("results.txt", "w");

	// Error Handling 2: Bogus Output File
	if(!outputf || !serviced){
		perror("Error: Unable to open one or more output file(s).\n");
		exit(EXIT_FAILURE);
	}

	// Initialize requestorID for the requestor pool
	pthread_t requestors;
	
	int producer = pthread_create(&requestors, NULL, (void*) requesterPool, inputFiles);
	if(producer != 0){
		errno = producer;
		perror("Error: pthread_create requestor\n");
		exit(EXIT_FAILURE);
	}


	pthread_t resolvers;

	int consumer = pthread_create(&resolvers, NULL, (void*) resolverPool, outputf);
	if(consumer != 0){
		errno = consumer;
		perror("Error: pthread_create resolver.\n");
		exit(EXIT_FAILURE);
	}

	// Suspend thread calls until execution is complete
	for(int i = 0; i < num_req; i ++){	
		pthread_join(requestors, NULL);
	}
	for(int i = 0; i< num_res; i++){
		pthread_join(resolvers, NULL);
	}
	
	gettimeofday(&end, NULL);
	printf("Number of Requestor Threads: %d\n", num_req);
	printf("Number of Resolver Threads: %d\n", num_res);
	printf("Execution Time: %ld nanoseconds.\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

	fclose(serviced);
	fclose(outputf);
	//fclose(perf);
	clean(&q);

	// Destroy condition vars and locks
	pthread_mutex_destroy(&arr_lock);
	pthread_mutex_destroy(&req_lock);
	pthread_mutex_destroy(&res_lock);
	pthread_cond_destroy(&arr_empty);
	pthread_cond_destroy(&arr_full);

	pthread_exit(NULL);

	return 0;
}
