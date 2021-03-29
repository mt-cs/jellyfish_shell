#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "elist.h"
// make util.c, update MakeFile
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

int main(void)
{
    init_ui();

    char *command = NULL;
    struct elist *list = elist_create(0, sizeof(char **)); 
    
    signal(SIGINT, SIG_IGN);

    while (true) {
        command = read_command();
        if (command == NULL) {
            break;
        }

        LOG("Input command: %s ", command);
        // 1. tokenize command
        char *next_tok = command;
        char *curr_tok;
        // 2. add each token to a list
        while ((curr_tok = next_token(&next_tok, " \t\n")) != NULL) {
            printf("adding: %s\n", curr_tok);
            elist_add(list, &curr_tok);
        }
        char *null = (char *) 0;
        elist_add(list, &null);

        LOG("Processed %zu tokens\n", elist_size(list));

        if (elist_size(list) == 1) {
            LOGP("Empty command\n");
            continue;
        }
        // 3. check for built-in functions
        // just an example
        // if (command[0] == '#') { 
        //     printf("comment!!\n");
        // } 
        char **first = elist_get(list, 0);

        if (strcmp(*first, "#") == 0) { 
            printf("comment!!\n");
        } 
      
        if (!strcmp(*first, "cd")){
            char **directory = elist_get(list, 1);
            chdir(*directory);
            if(chdir(*directory) == -1)
            {
                fprintf(stderr, "vsh: Error changing directory\n");
            }

        }      
        /*
        cd to change the CWD. cd without arguments should return to the user’s home directory.
        # (comments): strings prefixed with # will be ignored by the shell
        history, which prints the last 100 commands entered with their command numbers
        ! (history execution): entering !39 will re-run command number 39, and !! re-runs the last command that was entered. !ls re-runs the last command that starts with ‘ls.’ Note that command numbers are NOT the same as the array positions; e.g., you may have 100 history elements, with command numbers 600 – 699.
        jobs to list currently-running background jobs.
        exit to exit the shell.
        */

        // 4. preprocess command (before redirection "<>>", background "&")
        // this is the place to do pre-processing on the command line
        // if we have a > character in the tokens:
        //      open the file that came after the >

        
        

        // 5. fork a child process
        pid_t child = fork();
        if (child == -1) {
            perror("fork");
        } else if (child == 0) {
            /* I am the child */
            char **args = elist_get(list, 0);

            // Use dup2 to achieve this; 
            // right before the newly-created child process calls execvp, 
            // you will open the appropriate files and set up redirection with dup2.

        //      dup2(fd, fileno(stdout))
            execvp(args[0], args);
            perror("execvp");
            exit(0);
        } else {
            /* I am the parent */
            int status; //synchronize the processes
            wait(&status);
            printf("Child finished executing: %d\n", status);
            elist_clear(list);
        }
        // 6. execute whatever command the user asked for

        
        
        /* We are done with command; free it */
        free(command);
    }

    return 0;
}

/*call dup2, sigchld*/
/* our shell is an unkillable process 
 * ignore sigint */


