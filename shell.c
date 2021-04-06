/**
 * @file
 */

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
void jellyfish_process_command(char *command, struct elist *list);
int jellyfish_history(char **args);
int jellyfish_history_execution(char **args);
int jellyfish_jobs(char **args);
int jellyfish_io(int index, char **args, bool append, bool write, bool read);

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

int jellyfish_io(int index, char **args, bool append, bool write, bool read) {
    if(index != -1) { // means > < >> is in token
        args[index] = 0;
        int open_perms = 0666; // 0666 is octal notation for allowing all file
        int open_flags;
        int fd;
        if (append) {
            open_flags = O_RDWR | O_CREAT | O_APPEND;
            fd = open(args[index + 1], open_flags, open_perms);
            LOG("Filename >> append: %s\n", args[index + 1]);
            if (fd == -1) {
                perror("open");
                return 1;
            }
            dup2(fd, fileno(stdout));
            if (dup2(fd, fileno(stdout)) == -1) { 
                perror("dup2");
                return 1;
            }    
        } else if (write) { 
            open_flags = O_RDWR | O_CREAT | O_TRUNC;
            fd = open(args[index + 1], open_flags, open_perms);
            LOG("Filename > write: %s\n", args[index + 1]);
            if (fd == -1) {
                perror("open");
                return 1;
            }
            dup2(fd, fileno(stdout));
            if (dup2(fd, fileno(stdout)) == -1) { 
                perror("dup2");
                return 1;
            }    
        } else if (read) {
            open_flags = O_RDONLY;
            fd = open(args[index + 1], open_flags);
            LOG("Filename < read: %s\n", args[index + 1]);
            dup2(fd, fileno(stdin));
            if (dup2(fd, fileno(stdin)) == -1) { 
                perror("dup2");
                return 1;
            }
        }       
    }
    return 0;    
}

void jellyfish_process_command(char *command, struct elist *list) {
        
        // HISTORY
        if (strlen(command) > 0) {
            hist_add(command);
            LOG("Adding into history %s\n", command);
        }
     
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
            if (curr_tok[0] == '!') {
                LOGP("history execution...\n");
                LOG("comment afterward: %c\n", curr_tok[1]);
                const char *hist_cmd = hist_search_cnum(curr_tok[1]-'0');
                elist_add(list, &hist_cmd);
                LOG("Adding into elist %s\n", hist_cmd);
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
            //continue;
            return;
        } 
        // 3. check for built-in functions
        char **comm = elist_get(list, 0);
        if (comm[0] == NULL ){
            //continue;
            return;
        }
        
        // cd to change the CWD. cd without arguments should return to the user’s home directory.
        if (!strcmp(*comm, "cd")){
            char **dir = elist_get(list, 1);
            jellyfish_cd(dir);
        } 
        // exit to exit the shell.
        else if (!strcmp(*comm, "exit")){
            LOGP("exit...\n");
            //free(command);
            //return 0;
            exit(0);
            //TODO: there is a function exit that can be called from anywhere but it might cause memory leak
        }
        //history, which prints the last 100 commands entered with their command numbers
        else if (!strcmp(*comm, "history")){
            LOGP("history...\n");
            hist_print();
            fflush(stdout);
            //continue;
            return;
        }
        /*  
        ! (history execution): entering !39 will re-run command number 39, 
        and !! re-runs the last command that was entered. 
        !ls re-runs the last command that starts with ‘ls.’ 
        Note that command numbers are NOT the same as the array positions; 
        e.g., you may have 100 history elements, with command numbers 600 – 699.
        */
        // else if (!strcmp(*comm, "!")){
        //     LOGP("history execution...\n");
        //     char **eg = elist_get(list, 1);
        //     LOG("elist get 1 is: %s", &eg);
        //     //const char *hist_search = hist_search_cnum(elist_get(list, 1)); 
        //     //LOG("Repeat command: %s\n", hist_search);
        // }
        // else if (!strcmp(*comm, "!!")){
        //     LOGP("re-runs the last command that was entered.");
        // }
        // else if (!strcmp(*comm, "!ls")){
        //     LOGP("re-runs the last command that starts with ‘ls.’ ");
        
        // }
        // // jobs to list currently-running background jobs.
        // else if (!strcmp(*comm, "jobs")){
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
            ssize_t index = -1;

            //loop through the args
            for (int i = 0; i < elist_size(list) - 1; ++i) {
                bool write = false;
                bool read = false;
                bool append = false;
                if (strcmp(args[i], ">") == 0) {
                    index = i;
                    write = true;
                    jellyfish_io(index, args, append, write, read);
                    LOGP ("we are in >\n");
                } else if (strcmp(args[i], "<") == 0) {
                    index = i;
                    read = true;
                    jellyfish_io(index, args, append, write, read);
                    LOGP ("we are in <\n");
                } else if (strcmp(args[i], ">>") == 0) {
                    index = i;
                    append = true;
                    jellyfish_io(index, args, append, write, read);
                    LOGP ("we are in >>\n");
                }
            }   
            if(execvp(args[0], args) == -1) { // dump args to execvp
                perror("execvp"); // this will only execute if execvp fail
                close(fileno(stdin));
                close(fileno(stdout));
                close(fileno(stderr));
                exit(1);
            }
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

int main(void)
{
    init_ui();
    hist_init(100);
    //jobs_init();
    struct elist *list = elist_create(0, sizeof(char **));

    signal(SIGINT, SIG_IGN);
    //signal(SIGCHLD, jobs_sigchld);
    while (true) {
        char *command = read_command();
        if (command == NULL) {
            free(command);
            break;
        }
        LOG("Input command: %s\n", command);
        jellyfish_process_command(command, list);
        
        // // HISTORY
        // if (strlen(command) > 0) {
        //     hist_add(command);
        //     LOG("Adding into history %s\n", command);
        // }
        
        
        // // 1. tokenize command
        // char *next_tok = command;
        // char *curr_tok;

        // elist_clear(list);
        // // 2. add each token to a list
        // while ((curr_tok = next_token(&next_tok, " \t\n")) != NULL) {
        //     // # (comments): strings prefixed with # will be ignored by the shell
        //     if (curr_tok[0] == '#') {
        //         break;
        //     } 
        //     if (curr_tok[0] == '!') {
        //         LOGP("history execution...\n");
        //         LOG("comment afterward: %c\n", curr_tok[1]);
        //         const char *hist_cmd = hist_search_cnum(curr_tok[1]-'0');
        //         elist_add(list, &hist_cmd);
        //         LOG("Adding into elist %s\n", hist_cmd);
        //         break;
        //     }
        //     LOG("adding: %s\n", curr_tok);
        //     elist_add(list, &curr_tok); // &curr_tok we are copying the pointers to the cahar arrays, instead of just the first char array
        // }
        // char *null = (char *) 0; // set a null char and append it to the end of list
        // elist_add(list, &null); // we terminate the list of token with null
        // // the way execvp knows that we are at the end of the list of tokens (argument) of the command
        // // is when it hits a null. we are doing it in a funky way because that's how we deign our elist
        // // we pass a pointer into something and we memcpy it into place
        // // ls -l / null
        
        // LOG("Processed %zu tokens\n", elist_size(list));

        // if (elist_size(list) == 1) {
        //     LOGP("Empty command\n");
        //     continue;
        // } 
        // // 3. check for built-in functions
        // char **comm = elist_get(list, 0);
        // if (comm[0] == NULL ){
        //     continue;
        // }
        
        // // cd to change the CWD. cd without arguments should return to the user’s home directory.
        // if (!strcmp(*comm, "cd")){
        //     char **dir = elist_get(list, 1);
        //     jellyfish_cd(dir);
        // } 
        // // exit to exit the shell.
        // else if (!strcmp(*comm, "exit")){
        //     LOGP("exit...\n");
        //     //free(command);
        //     return 0;
        //     //TODO: there is a function exit that can be called from anywhere but it might cause memory leak
        // }
        // //history, which prints the last 100 commands entered with their command numbers
        // else if (!strcmp(*comm, "history")){
        //     LOGP("history...\n");
        //     hist_print();
        //     fflush(stdout);
        //     continue;
        // }
        // /*  
        // ! (history execution): entering !39 will re-run command number 39, 
        // and !! re-runs the last command that was entered. 
        // !ls re-runs the last command that starts with ‘ls.’ 
        // Note that command numbers are NOT the same as the array positions; 
        // e.g., you may have 100 history elements, with command numbers 600 – 699.
        // */
        // // else if (!strcmp(*comm, "!")){
        // //     LOGP("history execution...\n");
        // //     char **eg = elist_get(list, 1);
        // //     LOG("elist get 1 is: %s", &eg);
        // //     //const char *hist_search = hist_search_cnum(elist_get(list, 1)); 
        // //     //LOG("Repeat command: %s\n", hist_search);
        // // }
        // // else if (!strcmp(*comm, "!!")){
        // //     LOGP("re-runs the last command that was entered.");
        // // }
        // // else if (!strcmp(*comm, "!ls")){
        // //     LOGP("re-runs the last command that starts with ‘ls.’ ");
        
        // // }
        // // // jobs to list currently-running background jobs.
        // // else if (!strcmp(*comm, "jobs")){
        // //     LOGP("history");

        // // }
        
        

        // // 4. preprocess command (before redirection "<>>", background "&")
        // // this is the place to do pre-processing on the command line
        // // if we have a > character in the tokens:
        // //      open the file that came after the >
        

        // // 5. fork a child process
        // pid_t child = fork();
        // if (child == -1) {
        //     perror("fork");
        // } else if (child == 0) {
        //     /* I am the child */
        //     // args is pointing at the start of an array of pointer
        //     // if we change the list in anyway the original args will not be valid anymore
        //     char **args = elist_get(list, 0); // we get the first element, we will dump args to execvp
        //     ssize_t index = -1;

        //     //loop through the args
        //     for (int i = 0; i < elist_size(list) - 1; ++i) {
        //         bool write = false;
        //         bool read = false;
        //         bool append = false;
        //         if (strcmp(args[i], ">") == 0) {
        //             index = i;
        //             write = true;
        //             jellyfish_io(index, args, append, write, read);
        //             LOGP ("we are in >\n");
        //         } else if (strcmp(args[i], "<") == 0) {
        //             index = i;
        //             read = true;
        //             jellyfish_io(index, args, append, write, read);
        //             LOGP ("we are in <\n");
        //         } else if (strcmp(args[i], ">>") == 0) {
        //             index = i;
        //             append = true;
        //             jellyfish_io(index, args, append, write, read);
        //             LOGP ("we are in >>\n");
        //         }
        //     }   
        //     if(execvp(args[0], args) == -1) { // dump args to execvp
        //         perror("execvp"); // this will only execute if execvp fail
        //         close(fileno(stdin));
        //         close(fileno(stdout));
        //         close(fileno(stderr));
        //         exit(1);
        //     }
        // } else {
        //     /* I am the parent */
        //     int status; //synchronize the processes
        //     wait(&status);
        //     set_status(status);
        //     LOG("Child finished executing: %d\n", status);
        //     elist_clear(list); // if we don't have the parent waiting for the child anymore, that's effectively a background job
        // }
        // // 6. execute whatever command the user asked for
        // /* We are done with command; free it */
        // free(command);
    }
    //destroy_ui();
    //jobs_destroy();
    hist_destroy();
    elist_destroy(list);
    return 0;
}


