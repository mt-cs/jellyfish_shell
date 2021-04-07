/**
 * @file
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <signal.h>
#include <sys/types.h>

#include "logger.h"
#include "job.h"

void sigint_handler(int signo) {
    /*
     * (2) print the prompt only if no command is currently executing ?
     */
    printf("\n");
    printf("Goodbye 👋 🙃!\n");
    fflush(stdout);
    exit(0);
}

void child_signal_handler(int signo) {
    int status=0;
    pid_t wait_pid;
    LOGP("-----------------------------------------<3 child_signal_handler\n");
    wait_pid = waitpid(-1, &status, WNOHANG);
    LOG("Job num is: %d signo %d status %d\n", wait_pid, signo, WEXITSTATUS(status));
    job_remove(wait_pid);
}
