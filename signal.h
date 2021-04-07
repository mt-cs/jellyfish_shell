/**
 * @file
 */

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

/**
 * Set up our signal handler. SIGINT can be sent via Ctrl+C 
 * @param signo signal number
 */
void sigint_handler(int signo);

/**
 * Set up our signal handler for SIGCHLD
 * @param signum signal number
 */
void child_signal_handler(int signo);

#endif
