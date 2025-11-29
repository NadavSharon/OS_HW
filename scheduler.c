#include <stdlib.h>
#include <string.h>
#include "scheduler.h"
#include "process.h"

/* Static Helper Functions  */

static int get_current_index(scheduler_t *sched) {
    
    if (sched->running == NULL) {
        return -1;
    }

    return (int)(sched->running - sched->table);
}

static pcb_t *find_next_fcfs(scheduler_t *sched) {
    pcb_t *next = NULL;

    for (int i=0; i<sched->process_count; i++){

        if(sched->table[i].state == STATE_READY){

            if(next == NULL || sched->table[i].arrival_time < next->arrival_time){
                next = &sched->table[i];
            }
        }
    }
    return next;
}


static pcb_t *find_next_rr(scheduler_t *sched) {
    int current_idx = get_current_index(sched);
    int start_idx;
    
    if (current_idx == -1) {
        // No process is currently running, start from the beginning
        start_idx = 0;
    } else {
        // Start searching from the next process in the list
        start_idx = (current_idx + 1) % sched->process_count;
    }

    for (int i =0; i < sched->process_count; i++) {
        int idx = (start_idx + i) % sched->process_count;

        if (sched->table[idx].state == STATE_READY) {
            return &sched->table[idx];
        }
    }
    return NULL; // No READY process found
}

/*
 * Creates and initializes a scheduler.
 */
scheduler_t *scheduler_create(pcb_t *processes, int process_count, algorithm_t alg, int quantum){

    scheduler_t *sched = malloc(sizeof(scheduler_t)); // Allocate memory for scheduler

    if (!sched) {
        return NULL;
    }

    memset(sched, 0, sizeof(scheduler_t)); //Reset array values

    sched->table = processes;
    sched->process_count = process_count;
    sched->alg = alg;
    sched->quantum = quantum;

    return sched;
}

void scheduler_step(scheduler_t *sched){

    if (!sched) {
        return;
    }

    // Implementation of the step function would go here
}