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

/**
 * If a command ends in &, then it should run in the background. 
 * In other words, don’t wait for the command to finish before 
 * prompting for the next command. 
 * If you enter jobs, your shell should print out 
 * a list of currently-running backgrounded processes 
 * (use the original command line as it was entered, including the & character). 
 * The status of background jobs is not shown in the prompt.

To implement this, you will need:

A signal handler for SIGCHLD. This signal is sent to a process any time one of its children exit.
A non-blocking call to waitpid in your signal handler. Pass in pid = -1 and options = WNOHANG.
This tells your signal handler the PID of the child process that exited. 
If the PID is in your jobs list, then it can be removed.

The difference between a background job and a regular job is simply whether 
or not a blocking call to waitpid() is performed. 
If you do a standard waitpid() with options = 0, then the job will run in the foreground 
and the shell won’t prompt for a new command until the child finishes (the usual case). 
Otherwise, the process will run and the shell will prompt for the next command without waiting.

NOTE: your shell prompt output may print in the wrong place when using background jobs. This is completely normal. 
 */

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
    
}

void job_remove(pid_t pid);

