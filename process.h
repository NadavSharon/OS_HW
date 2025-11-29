#ifndef PROCESS_H
#define PROCESS_H

/*
 * Process state values used by the scheduler.
 */
typedef enum {
    STATE_NEW,       /* Process has not yet arrived */
    STATE_READY,     /* Ready to run */
    STATE_RUNNING,   /* Currently executing on the CPU */
    STATE_BLOCKED,   /* Performing I/O */
    STATE_TERMINATED   /* Completed execution */
} state_t;

/*
 * Process Control Block (PCB)
 *
 * This structure contains all information required by the scheduler
 * to simulate process execution, I/O, and to compute scheduling metrics.
 *
 * Fields:
 *
 * pid              - unique process identifier
 * arrival_time     - time at which the process becomes READY
 * cpu_burst        - total CPU time required by the process
 * remaining_time   - CPU time remaining until completion
 *
 * is_io_bound      - nonzero if the process performs periodic I/O
 * io_frequency     - CPU units between I/O operations
 * io_duration      - time spent in BLOCKED per I/O event
 * io_counter       - CPU units executed since last I/O
 * io_remaining     - remaining time in current BLOCKED state
 *
 * state            - current scheduling state of the process
 *
 * first_run_time   - time when the process first enters RUNNING
 * completion_time  - time when the process enters TERMINATED
 * waiting_time     - total time spent in READY state
 */
typedef struct {
    int pid;
    int arrival_time;
    int cpu_burst;
    int remaining_time;

    int is_io_bound;
    int io_frequency;
    int io_duration;
    int io_counter;
    int io_remaining;

    state_t state;

    int first_run_time;
    int completion_time;
    int waiting_time;
} pcb_t;

#endif
