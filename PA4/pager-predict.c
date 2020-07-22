/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */
#define EMPTY -1
#include <stdio.h> 
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "simulator.h"

int lru_global(int timestamps[MAXPROCESSES][MAXPROCPAGES], Pentry q[MAXPROCESSES], int *lru_proc, int *lru_page){
	int proc, page;
	int min_tick = INT_MAX;
	int tmp = 1;
	for(proc = 0; proc < MAXPROCESSES; proc++){
		for(page = 0; page < MAXPROCPAGES; page++){
			if(q[proc].pages[page] == 1 && timestamps[proc][page] < min_tick){
				min_tick = timestamps[proc][page];
				*lru_page = page;
				*lru_proc = proc;
				tmp = 0;
			}
		}
	}
	return tmp;
}

int lru_local(int timestamps[MAXPROCESSES][MAXPROCPAGES], Pentry q[MAXPROCESSES], int proc, int *lru_page){
	int page;
	int min_tick = INT_MAX;
	int tmp = 1;
	for(page = 0; page < MAXPROCESSES; page++){
		if(q[proc].pages[page] == 1 && timestamps[proc][page] < min_tick){
			*lru_page = page;
			min_tick = timestamps[proc][page];
			tmp = 0;
		}
	}
	return tmp;
}

int page_calc(int pc){
	return pc/PAGESIZE;
}

struct page_info{
	int page;
	int count;
	int *timestamp;
};
typedef struct page_info pageInfo;

void config(pageInfo cfg[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES]){
	for(int i = 0; i < MAXPROCESSES; i++){
		for(int j = 0; j < MAXPROCESSES; j++){
			for(int k = 0; k < MAXPROCESSES; k++){
				cfg[i][j][k].page = EMPTY;
				cfg[i][j][k].count = -1;
				cfg[i][j][k].timestamp = NULL;
			}	
		}
	}
}

/* Markov Chain logic: for each current page (p_t), keep track of the previous page (p_t-1) */
void insert_config(int cur_page, int proc, int prev_page, 
			pageInfo cfg[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES],
			int timestamps[MAXPROCESSES][MAXPROCPAGES]){
	pageInfo *blocks;
	blocks = cfg[proc][prev_page];

	for(int i = 0; i< MAXPROCPAGES; i++){
		if(blocks[i].page == cur_page){
			blocks[i].count++;
			break;
		}
		if(blocks[i].page == EMPTY){
			blocks[i].page = cur_page;
			blocks[i].count = 1;
			blocks[i].timestamp = &(timestamps[proc][i]);
			break;
		}
	}
}

pageInfo* predict_page(int proc, int pc, pageInfo cfg[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES]){
	return cfg[proc][page_calc(pc+101)];
}

int count_pages(pageInfo *pages){
	int n = 0;
	while(pages[n].page != EMPTY && n < MAXPROCPAGES){
		n++;
	}
	return n;
}

void swap_pages(pageInfo *left, pageInfo *right){
	pageInfo tmp = *left;
	*left = *right;
	*right = tmp;
}

// Most-Recently Used Sort: Descending based on timestamps
void sort_pages(pageInfo *pages){
	int n = count_pages(pages);
	int swapped = 0;
	do{
	    swapped = 0;
		for(int i = 1; i < n; i++){
			assert(pages[i-1].timestamp != NULL && pages[i].timestamp != NULL);
			if(*(pages[i-1].timestamp) < *(pages[i].timestamp)){
				swap_pages(pages+(i-1), pages+i);
				swapped = 1;
			}
		}
	}
	while(swapped);
}


void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    static int procs[MAXPROCESSES];
    static int prev_pc[MAXPROCESSES];

    static pageInfo cfg[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
    
    /* Local vars */
    int proctmp;
    int pagetmp;
    int lru_page;
    int prev_page;
    int cur_page;

    /* initialize static vars on first run */
    if(!initialized){
	/* Init complex static vars here */
	config(cfg);
	for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
		for(pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++){
			timestamps[proctmp][pagetmp] = 0;
		}
		procs[proctmp] = 0;
	}
	initialized = 1;
    }
    
    /* Update control flow */
    for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
	// If inactive, then continue
	if(!q[proctmp].active){ continue; }
	// If previous page is -1, then continue
	if(prev_page == -1){ continue; }
	prev_page = page_calc(prev_pc[proctmp]);
	// Update the previous program counter
	prev_pc[proctmp] = q[proctmp].pc;
	// If previous is the same as current, then continue
	cur_page = page_calc(q[proctmp].pc);
	if(prev_page == cur_page){ continue; }
	pageout(proctmp, prev_page);
	insert_config(cur_page, proctmp, prev_page, cfg, timestamps);
    }

    // Update timestamps for active processes
    for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
	if(!q[proctmp].active){continue;}
	pagetmp = (q[proctmp].pc-1)/PAGESIZE;
        timestamps[proctmp][pagetmp] = tick;
    }	

    for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
	if(!q[proctmp].active){
		for(pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++){
			pageout(proctmp, pagetmp);
		}
		continue;
    	}
	// Find the next needed page
	pagetmp = (q[proctmp].pc)/PAGESIZE;
	// If swapped in, then continue
	if(q[proctmp].pages[pagetmp] == 1){ continue;}
	if(pagein(proctmp, pagetmp)){
		// Process no longer waiting on the pageout()
		procs[proctmp] = 0;
		continue;
	}
	if(procs[proctmp]){continue;}
	// If page is needed, but all are taken, then find the LRU page to swap with current
	if(lru_local(timestamps, q, proctmp, &lru_page)){continue;}
	if(!pageout(proctmp, lru_page)){
		fprintf(stderr, "ERROR: Paging out LRU page failed!\n");
		exit(EXIT_FAILURE);
	}
	// Signal that this process is waiting for a pageout()
	procs[proctmp] = 1;
    }
    for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
	pageInfo *predictions;
	if(!q[proctmp].active){continue;}
	predictions = predict_page(proctmp, q[proctmp].pc, cfg);
	sort_pages(predictions);
	for(int i = 0; i < count_pages(predictions); i++){
		pagein(proctmp, predictions[i].page);
	}
    }

    /* advance time for next pageit iteration */
    tick++;
} 
