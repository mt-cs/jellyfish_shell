/**
 * @file
 *
 * Background jobs functionality
 */

#ifndef _JOB_H_
#define _JOB_H_

#include <sys/types.h>

/**
 * Initializes the job; sets up elist data structures
 */
void job_init();

/**
 * Creates a new elist data structure. If the initial size is set to 0, the
 * default capacity will be used. 
 *
 * @param init_capacity The initial capacity of the list
 * @param item_sz Size of the elements that will be stored in the list (in bytes)
 */
void job_add(size_t list_sz, size_t item_sz);

/**
 * Destroys the specified list and frees any memory that was allocated to it.
 */
void job_destroy(void);

#endif