/**
 * @file
 */

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stddef.h>

#include "logger.h"
#include "elist.h"
#include "job.h"

struct elist *jobs_list;

void job_init() 
{
    jobs_list = elist_create(0, sizeof(struct job));
}

void job_add(struct job job_struct) 
{
    elist_add(jobs_list, &job_struct);
}

void job_destroy(void)
{
    elist_destroy(jobs_list);
}

void job_print(void) {
    void *curr_job;
    size_t index = 0;
    
    while ((curr_job = elist_get(jobs_list, index)) != NULL) {
        struct job *jobs = curr_job;
        printf("%s\n", jobs->command);
        index++;
    }
}

void job_remove(pid_t pid) {
    void *curr_job;
    size_t index = 0;
    while ((curr_job = elist_get(jobs_list, index)) != NULL) {
        struct job *jobs = curr_job;
        LOG("checking pid %d index %zu\n", jobs->pid, index);
        if (jobs->pid == pid) {
            LOGP("pid found in list, removing...\n");
            elist_remove(jobs_list, index);
            return;
        }
        index++;
    }
}

