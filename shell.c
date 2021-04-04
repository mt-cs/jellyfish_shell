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
#include <limits.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "elist.h"
#include "util.h"

/*
  Function Declarations for builtin shell commands:
 */
void jellyfish_cd(char **args);
int jellyfish_history(char **args);
int jellyfish_history_execution(char **args);
int jellyfish_jobs(char **args);

int jellyfish_history(char **args) {
    return 0;
}

int jellyfish_history_execution(char **args) {
    return 0;
}

int jellyfish_jobs(char **args) {
    return 0;
}

void jellyfish_cd(char **dir)
{
    LOGP("change directory...\n");
    chdir(*dir);
    if (*dir == NULL || strcmp(*dir, "~" ) == 0) {
        chdir(getenv("HOME")); 
    }
    else { 
        if (chdir(*dir) == -1) {
            LOGP("no such directory\n");
        }
    }
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
            free(command);
            break;
        }

        LOG("Input command: %s\n", command);
        // 1. tokenize command
        char *next_tok = command;
        char *curr_tok;

        elist_clear(list);
        // 2. add each token to a list
        while ((curr_tok = next_token(&next_tok, " \t\n")) != NULL) {
            // # (comments): strings prefixed with # will be ignored by the shell
            if (curr_tok[0] == '#') {
                break;
            } 
            LOG("adding: %s\n", curr_tok);
            elist_add(list, &curr_tok); // &curr_tok we are copying the pointers to the cahar arrays, instead of just the first char array
        }
        char *null = (char *) 0; // set a null char and append it to the end of list
        elist_add(list, &null); // we terminate the list of token with null
        // the way execvp knows that we are at the end of the list of tokens (argument) of the command
        // is when it hits a null. we are doing it in a funky way because that's how we deign our elist
        // we pass a pointer into something and we memcpy it into place
        // ls -l / null
        
        LOG("Processed %zu tokens\n", elist_size(list));

        if (elist_size(list) == 1) {
            LOGP("Empty command\n");
            continue;
        }
        // 3. check for built-in functions
        char **comm = elist_get(list, 0);
        if (comm[0] == NULL ){
            continue;
        }
        
        // cd to change the CWD. cd without arguments should return to the user’s home directory.
        if (!strcmp(*comm, "cd")){
            char **dir = elist_get(list, 1);
            jellyfish_cd(dir);
        } 
        // exit to exit the shell.
        else if (!strcmp(*comm, "exit")){
            LOGP("exit\n");
            return 0;
            //TODO: there is a function exit that can be called from anywhere but it might cause memory leak
        }
        // //history, which prints the last 100 commands entered with their command numbers
        // else if (!strcmp(*first, "history")){
        //     LOGP("history");
        // }
        // /*  
        // ! (history execution): entering !39 will re-run command number 39, 
        // and !! re-runs the last command that was entered. 
        // !ls re-runs the last command that starts with ‘ls.’ 
        // Note that command numbers are NOT the same as the array positions; 
        // e.g., you may have 100 history elements, with command numbers 600 – 699.
        // */
        // else if (!strcmp(*first, "!")){
        //     LOGP("history execution");
        // }
        // else if (!strcmp(*first, "!!")){
        //     LOGP("re-runs the last command that was entered.");
        // }
        // else if (!strcmp(*first, "!ls")){
        //     LOGP("re-runs the last command that starts with ‘ls.’ ");
        // }
        // // jobs to list currently-running background jobs.
        // else if (!strcmp(*first, "jobs")){
        //     LOGP("history");
        // }
        
        

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
            // args is pointing at the start of an array of pointer
             // if we change the list in anyway the original args will not be valid anymore
            char **args = elist_get(list, 0); // we get the first element, we will dump args to execvp
            LOG("args is: %s\n", *args);
            LOG("args[0] is: %s\n", args[0]);
            // Use dup2 to achieve this; 
            // right before the newly-created child process calls execvp, 
            // you will open the appropriate files and set up redirection with dup2.
            // example:
            // echo hello > /tmp/test_file
            // put hello in /tmp/test_file (after the >)
            
            //ssize_t index = elist_index_of(list, ">");
            ssize_t index = -1;

            for (int i = 0; i < elist_size(list) - 1; ++i) {

                if (strcmp(args[i], ">") == 0) {
                    index = i;
                    int open_flags = O_RDWR | O_CREAT | O_TRUNC;

                    int open_perms = 0666;

                    int fd = open(args[index + 1], open_flags, open_perms);
                    if (fd == -1) {
                        perror("open");
                        return 1;
                    }
                    dup2(fd, fileno(stdout));
                    if(dup2(fd, fileno(stdout)) == -1) {
                        perror("dup2");
                        return 1;
                    }

                } else if (strcmp(args[i], ">>") == 0){
                    index = i;

                    int open_flags = O_RDWR | O_CREAT | O_APPEND;   //---> changed to O_APPEND
                        int open_perms = 0666;
                    
                        int fd = open(args[index + 1], open_flags, open_perms);
                        if (fd == -1) {
                            perror("open");
                            return 1;
                        }

                        if (dup2(fd, fileno(stdout)) == -1) {
                            perror("dup2");
                            return 1;
                    }

                } else if (strcmp(args[i], "<") == 0){
                    index = i;

                    int open_flags = O_RDWR | O_CREAT | O_TRUNC;  
                        int open_perms = 0666;
                    
                        int fd = open(args[index + 1], open_flags, open_perms);
                        if (fd == -1) {
                            perror("open");
                            return 1;
                        }

                        if (dup2(fd, fileno(stdin)) == -1) {
                            perror("dup2");
                            return 1;
                        }
                }
            
            }

            //loop through the args
            // for (int i = 0; i < elist_size(list) - 1; ++i) {
            //     LOG("args[i] = %s\n", args[i]);
            //     if (strcmp(args[i], ">") == 0) {
            //         index = i;
            //         LOG("Index > is: %ld\n", index);
            //         LOG("Args > at index is: %s\n", args[index]);
            //         break;
            //     } else if (strcmp(args[i], "<") == 0) {
            //         index = i;
            //         LOG("Index < is: %ld\n", index);
            //         LOG("Args < at index is: %s\n", args[index]);
            //         break;
            //     } else {
            //         LOGP("We are in the else clause\n");
            //     }
            // }

            // LOG("Index now is: %ld\n", index);
            // char **redirect = elist_get(list, index);
            // LOG("redirect inside if is %s\n", *redirect);
            // if(index != -1) { // means > is in token
            //     args[index] = 0;
            //     int open_flags = O_RDWR | O_CREAT | O_TRUNC;
            //     /**
            //     * These are the file permissions we see when doing an `ls -l`: we can
            //     * restrict access to the file. 0666 is octal notation for allowing all file
            //     * permissions, which is then modified by the user's 'umask.'
            //     */
            //     int open_perms = 0666;

            //     int fd = open(args[index + 1], open_flags, open_perms);
            //     if (fd == -1) {
            //         perror("open");
            //         return 1;
            //     }
                
            //     LOG("redirect inside if is %s\n", *redirect);
            //     if ((strcmp(*redirect, ">") == 0)) {
            //         dup2(fd, fileno(stdout));
            //         if (dup2(fd, fileno(stdout)) == -1) { 
            //             perror("dup2");
            //             return 1;
            //         }
            //     } else if ((strcmp(*redirect, "<") == 0)) {
            //         dup2(fd, fileno(stdin));
            //         if (dup2(fd, fileno(stdin)) == -1) { 
            //             perror("dup2");
            //             return 1;
            //         }
            //     }       
            // } else{
            //     LOGP("No > or < token");
            // }

            

            if(execvp(args[0], args) == -1) { // dump args to execvp
                perror("execvp"); // this will only execute if execvp fail
                close(fileno(stdin));
                close(fileno(stdout));
                close(fileno(stderr));
                exit(1);
            }
            exit(0); // why exit(0) shell 194?
        } else {
            /* I am the parent */
            int status; //synchronize the processes
            wait(&status);
            set_status(status);
            LOG("Child finished executing: %d\n", status);
            elist_clear(list); // if we don't have the parent waiting for the child anymore, that's effectively a background job
        }
        // 6. execute whatever command the user asked for
        /* We are done with command; free it */
        free(command);
    }
    elist_destroy(list);
    return 0;
}

/*call dup2, sigchld*/
/* our shell is an unkillable process 
 * ignore sigint */


