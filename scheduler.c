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

    pcb_t *current;
    pcb_t *next = NULL;
    int i;
    if (!sched) {
        return;
    }
    // Step 1&2: Handle arrivals and I/O completions
    for (i = 0; i < sched->process_count; i++){
        current = &sched->table[i];
        
        // Handle process arrivals
        if (current->state == STATE_NEW && current->arrival_time == sched->time){
            current->state = STATE_READY;
        }

        // Handle I/O completion
        if (current->state == STATE_BLOCKED){
            // Decrement remaining I/O time
            current->io_remaining--;
            // Check if I/O is complete
            if (current->io_remaining == 0){
                current->state = STATE_READY;
            }
        }
    }
    
    // Step 3: Select next process to run if CPU is idle
    if (sched->running == NULL){

        if(sched->alg == ALG_FCFS){
            next = find_next_fcfs(sched);
        }

        else if(sched->alg == ALG_RR){
            next = find_next_rr(sched);
        }

        if (next != NULL){
            //Load next process into CPU
            sched->running = next;
            next->state = STATE_RUNNING;

            //Reset time in quantum for RR
            sched->time_in_quantum = 0;

            // Record first run time
            if(next->remaining_time == next->cpu_burst){
                next->first_run_time = sched->time;
            }
        }
    }
    // Step 4: Execute the running process
    if (sched->running != NULL){
        pcb_t *running = sched->running;
        
        //Update counters
        running->remaining_time--;
        running->io_counter++;
        sched->busy_time++;
        sched->time_in_quantum++;

        if (running->remaining_time ==0){
            running->state = STATE_TERMINATED;
            running->completion_time = sched->time + 1; // Finish at end of this time unit
            sched->running = NULL; // CPU becomes idle
        }
        else if (running->is_io_bound && running->io_counter >= running->io_frequency){
            running->state = STATE_BLOCKED;
            running->io_remaining = running->io_duration;
            running->io_counter = 0;
            sched->running = NULL; // CPU becomes idle
        }
        else if (sched->alg == ALG_RR && sched->time_in_quantum >= sched->quantum){
            running->state = STATE_READY;
            sched->running = NULL; // CPU becomes idle
        }
    }
    // Step 5: Update waiting times for READY processes
    for (i = 0; i < sched->process_count; i++){
        current = &sched->table[i];
        if (current->state == STATE_READY){
            current->waiting_time++;
        }
    }
    sched->time++; // Increment simulation time
}