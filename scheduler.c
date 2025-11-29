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

    int start_idx = (get_current_index(sched)+1) % sched->process_count; //if no running process, start from 0
    
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

    if (!sched)
        return NULL;

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

    if (!sched)
        return;

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

        //Check for process state changes
        if (running->remaining_time ==0){
            running->state = STATE_TERMINATED;
            running->completion_time = sched->time + 1; // Finish at end of this time unit
            sched->running = NULL; // CPU becomes idle
        }
        //Check for I/O blocking
        else if (running->is_io_bound && running->io_counter >= running->io_frequency){
            running->state = STATE_BLOCKED;
            running->io_remaining = running->io_duration;
            running->io_counter = 0;
            sched->running = NULL; // CPU becomes idle
        }
        //Check for quantum expiration
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

int scheduler_all_finished(scheduler_t *sched){
    int i;

    if (!sched) 
        return 1;

    for (i = 0; i < sched->process_count; i++){
        if (sched->table[i].state != STATE_TERMINATED){
            return 0; // Found a process not finished
        }
    }
    return 1; // All processes are finished
}

void scheduler_get_metrics(scheduler_t *sched, double *avg_wait, double *avg_turnaround, double *avg_response, double *cpu_util, double *throughput, double *fairness){

    // Handle NULL scheduler or zero processes
    if (!sched || sched->process_count == 0) 
        return;
    
    int i;
    double total_wait = 0;
    double total_turnaround = 0;
    double total_response = 0;
    double max_wait = 0;
    double min_wait = sched->table[0].waiting_time;


    for (i = 0; i < sched->process_count; i++){
        pcb_t *current = &sched->table[i];

        total_wait += current->waiting_time;
        total_turnaround += (current->completion_time - current->arrival_time);
        total_response += (current->first_run_time - current->arrival_time);

        if (current->waiting_time > max_wait){
            max_wait = current->waiting_time;
        }
        if (current->waiting_time < min_wait){
            min_wait = current->waiting_time;
        }
    }

    // Calculate metrics (depending on non-NULL pointers)
    if (avg_wait)
        *avg_wait = total_wait / sched->process_count;

    if (avg_turnaround)
        *avg_turnaround = total_turnaround / sched->process_count;

    if (avg_response)
        *avg_response = total_response / sched->process_count;

    if (cpu_util){
        if(sched->time > 0)
            *cpu_util = (double)sched->busy_time / sched->time;
        else
            *cpu_util = 0.0;
    }

    if (throughput){
        if(sched->time > 0)
            *throughput = (double)sched->process_count / sched->time;
        else
            *throughput = 0.0;
    }

    if (fairness)
        *fairness = max_wait - min_wait;
}

void scheduler_destroy(scheduler_t *sched){

    if (!sched)
        return;

    free(sched);
}


