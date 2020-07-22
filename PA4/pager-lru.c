/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

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


void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];
    static int procs[MAXPROCESSES];

    /* Local vars */
    int proctmp;
    int pagetmp;
    int lru_page;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	initialized = 1;
    }
    
    /* Update timestamps for active processes */
    for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
	if(!q[proctmp].active){
		continue;
	}
	pagetmp = (q[proctmp].pc-1)/PAGESIZE;
	timestamps[proctmp][pagetmp] = tick;
    }

    /*  */
    for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
	if(!q[proctmp].active){
		for(pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++){
			pageout(proctmp, pagetmp);
		}
		continue;
	}
	    /* Find the next page that the process will need */
	    pagetmp = (q[proctmp].pc)/PAGESIZE;
	    // If the page is swapped in, then skip it.
	    if(q[proctmp].pages[pagetmp] == 1){
		continue;}
	    // Otherwise, try to swap the page in. If success, continue.
	    if(pagein(proctmp, pagetmp)){
		procs[proctmp] = 0;
		continue;
	    }
	    // Don't pageout() if the current process is already being paged out
	    if(procs[proctmp]){
		continue;}
	    /* If the process need the page, but all frames are taken, then get the LRU page and swap it out
		for the new page */
	    if(lru_local(timestamps, q, proctmp, &lru_page)){
		continue;
	    }
	    if(!pageout(proctmp, lru_page)){
		fprintf(stderr, "ERROR: Paging out LRU page failed!\n");
		exit(EXIT_FAILURE);
	    }
	    // Signal that the current process is waiting for a pageout()
	    procs[proctmp] = 1;
	}

    /* advance time for next pageit iteration */
    tick++;
} 
