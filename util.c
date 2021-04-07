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

char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }
    return current_ptr;
}

void sigint_handler(int signo) {
    /*
     * making sure ^C doesn’t terminate your shell. 
     * To make the output look like the example above, 
     * in your signal handler you can 
     * (1) print a newline character, 
     * (2) print the prompt only if no command is currently executing, and 
     * (3) fflush(stdout).
     */
    printf("\n");
    printf("Goodbye 👋 🙃!\n");
    fflush(stdout);
    exit(0);
}

void child_signal_handler(int signo) {
    // get the return
    // that's the child
    // loop through find the job number
    int job_num = waitpid(-1, NULL, WNOHANG) > 0;
    LOG("Job num is: %d\n", job_num);
}
