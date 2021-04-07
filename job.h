/**
 * @file
 *
 * Background jobs functionality
 */

#ifndef _JOB_H_
#define _JOB_H_

#include <sys/types.h>

struct job
{
    pid_t pid;
    char command[256];
};

/**
 * Initializes the job list; sets up elist data structures.
 */
void job_init();

/**
 * Add job struct to an elist data structure.
 *
 * @param job_struct element to be added
 */
void job_add(struct job job_struct);

/**
 * Destroys the specified job list and frees any memory that was allocated to it.
 */
void job_destroy(void);

/**
 * Print jobs in list
 */
void job_print(void);

/**
 *  Remove a particular job in list
 *  @param pid pis_t id of the job to be removed
 */
void job_remove(pid_t pid);

#endif