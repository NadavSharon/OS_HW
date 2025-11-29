#include "scheduler.h"

int main() {
    /* Define a small set of processes */
    pcb_t table[2] = {
        {   /* Process 0 */
            .pid = 0,
            .arrival_time = 0,
            .cpu_burst = 5,
            .remaining_time = 5,
            .is_io_bound = 0,
            .io_frequency = 0,
            .io_duration = 0,
            .io_counter = 0,
            .io_remaining = 0,
            .state = STATE_NEW,
            .first_run_time = -1,
            .completion_time = -1,
            .waiting_time = 0
        },

        {   /* Process 1 */
            .pid = 1,
            .arrival_time = 3,
            .cpu_burst = 4,
            .remaining_time = 4,
            .is_io_bound = 0,
            .io_frequency = 0,
            .io_duration = 0,
            .io_counter = 0,
            .io_remaining = 0,
            .state = STATE_NEW,
            .first_run_time = -1,
            .completion_time = -1,
            .waiting_time = 0
        }
    };

    /* Create an FCFS scheduler */
    scheduler_t *sched =
        scheduler_create(table, 2, ALG_FCFS, 0);

    /* Run the simulation */
    while (!scheduler_all_finished(sched)) {
        scheduler_step(sched);
    }

    /* Read results */
    double wait, turn, resp, util, thr, fair;
    scheduler_get_metrics(sched,
                          &wait, &turn, &resp,
                          &util, &thr, &fair);

    scheduler_destroy(sched);
}
