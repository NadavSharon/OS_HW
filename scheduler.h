#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

/* Supported scheduling algorithms */
typedef enum {
    ALG_FCFS,   /* First Come First Served */
    ALG_RR      /* Round Robin */
} algorithm_t;

/* Scheduler object */
typedef struct {
    pcb_t *table;           /* Array of PCBs (owned by caller) */
    int process_count;      /* Number of processes */

    int time;               /* Current simulation time */
    int quantum;            /* Quantum size (RR only) */
    algorithm_t alg;        /* Selected scheduling algorithm */

    pcb_t *running;         /* Currently running process, or NULL */
    int time_in_quantum;    /* Time spent in current RR quantum */

    int busy_time;          /* Total time CPU was not idle */
} scheduler_t;

/*
 * Creates and initializes a scheduler.
 *
 * processes      - array of PCBs describing all processes
 * process_count  - number of processes in the array
 * alg            - selected scheduling algorithm (FCFS or RR)
 * quantum        - quantum length (must be > 0 for RR; ignored for FCFS)
 *
 * Returns: pointer to a scheduler_t on success, or NULL on failure.
 *          The function must not print or exit.
 */
scheduler_t *scheduler_create(pcb_t *processes,
                              int process_count,
                              algorithm_t alg,
                              int quantum);

/*
 * Advances the simulation by one time unit.
 * Updates process states, I/O behavior, READY queues,
 * RR quantum accounting, and per-process statistics.
 *
 * If sched is NULL, the function returns immediately.
 */
void scheduler_step(scheduler_t *sched);

/*
 * Returns 1 if all processes are in STATE_FINISHED.
 * Returns 0 otherwise.
 * If sched is NULL, returns 1.
 */
int scheduler_all_finished(scheduler_t *sched);

/*
 * Computes scheduling metrics:
 *
 * avg_wait        - average waiting time
 * avg_turnaround  - average turnaround time
 * avg_response    - average response time
 * cpu_util        - busy_time / total_time
 * throughput      - completed_processes / total_time
 * fairness        - max(waiting_time) - min(waiting_time)
 *
 * Any pointer may be NULL and will be ignored.
 * If sched is NULL, no values are written.
 */
void scheduler_get_metrics(scheduler_t *sched,
                           double *avg_wait,
                           double *avg_turnaround,
                           double *avg_response,
                           double *cpu_util,
                           double *throughput,
                           double *fairness);

/*
 * Frees scheduler resources.
 * Does nothing if sched is NULL.
 * Does not free the PCB array, which is owned by the caller.
 */
void scheduler_destroy(scheduler_t *sched);

#endif
