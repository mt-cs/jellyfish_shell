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
 * Initializes the job list; sets up elist data structures
 */
void job_init();

/**
 * Creates a new elist data structure. If the initial size is set to 0, the
 * default capacity will be used. 
 *
 * @param type element to be added
 */
void job_add(struct job job_struct);

/**
 * Destroys the specified list and frees any memory that was allocated to it.
 */
void job_destroy(void);

void job_print(void);

void job_remove(pid_t pid);

#endif