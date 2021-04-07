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
#include <ctype.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "elist.h"
#include "util.h"
#include "job.h"
#include "signal.h"

/*
 * Function Declarations for builtin shell commands
 */

bool jellyfish_built_in(struct elist *list);
int jellyfish_io(int index, char **args, bool append, bool write, bool read);
void jellyfish_run_io(struct elist *list, char **args);
void jellyfish_cd(struct elist *list);
void jellyfish_execute(struct elist *list);
void jellyfish_process_command(char *command, struct elist *list, bool repeat);
void jellyfish_history_execution(struct elist *list, char **comm);
void jellyfish_history();
void jellyfish_exit();
void jellyfish_jobs();

void jellyfish_history() {
    LOGP("history...\n");
    hist_print();
    fflush(stdout);
}

void jellyfish_exit(){
    LOGP("exit...\n");
    exit(0);
}

void jellyfish_history_execution(struct elist *list, char **comm) {
    LOGP("history execution...\n");
    char rest_of_command[256];
    strcpy(rest_of_command, &((*comm)[1]));
    char *tmp;
    const char *hist_search = NULL;
    if (isdigit(rest_of_command[0])) 
    {
        long command_num = strtol(rest_of_command, &tmp, 10);
        LOG("Command number: %ld\n", command_num);
        hist_search = hist_search_cnum(command_num - 1);
    } 
    else if (strncmp(rest_of_command, "!", 1) == 0) 
    {
        hist_search = hist_search_cnum(hist_last_cnum());
        LOG("Last command: %s\n", hist_search);
    } 
    else 
    {
        hist_search= hist_search_prefix(rest_of_command);
        LOG("Rest of command: %s\n", rest_of_command);
        LOG("Command prefix: %s\n", hist_search);
    }

    if (hist_search != NULL)
    {
        char *hist_dup = strdup(hist_search);
        jellyfish_process_command(hist_dup, list, true);
    }
    else {
        LOGP("Repeat command not found :(\n");
    }
}

void jellyfish_jobs() {
    LOGP("background jobs...\n");
    job_print();
}   

void jellyfish_cd(struct elist *list)
{
    char **dir = elist_get(list, 1);
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
    if(index != -1) { 
        args[index] = 0;
        int open_perms = 0666; // 0666 is octal notation for allowing all file
        int open_flags;
        int fd;
        if (append) 
        {
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
        } 
        else if (write) 
        { 
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
        } 
        else if (read) 
        {
            open_flags = O_RDONLY;
            fd = open(args[index + 1], open_flags);
            LOG("Filename < read: %s\n", args[index + 1]);
            dup2(fd, fileno(stdin));
            if (dup2(fd, fileno(stdin)) == -1) { 
                perror("dup2");
                return 1;
            }
        }
        close(fd);       
    }
    return 0;    
}

void jellyfish_run_io(struct elist *list, char **args) {
    ssize_t index;

    for (int i = 0; i < elist_size(list) - 1; ++i) {
        bool write = false;
        bool read = false;
        bool append = false;
        if (strcmp(args[i], ">") == 0) 
        {
            index = i;
            write = true;
            jellyfish_io(index, args, append, write, read);
            LOGP ("we are in >\n");
        } 
        else if (strcmp(args[i], "<") == 0) 
        {
            index = i;
            read = true;
            jellyfish_io(index, args, append, write, read);
            LOGP ("we are in <\n");
        } 
        else if (strcmp(args[i], ">>") == 0) 
        {
            index = i;
            append = true;
            jellyfish_io(index, args, append, write, read);
            LOGP ("we are in >>\n");
        } 
    } 
}

void jellyfish_execute(struct elist *list) {
    char **args = elist_get(list, 0);
    pid_t child = fork();
    if (child == -1) 
    {
        perror("fork");
    } 
    else if (child == 0) 
    {
        /* I am the child */
        char* last_token = args[elist_size(list) - 2];
        if (last_token[strlen(last_token) - 1] == '&') {
            LOGP("GETTING BACKGROUND JOB IN CHILD\n");
            elist_remove(list, elist_size(list) - 2);
        }
        jellyfish_run_io(list, args);  
        if(execvp(args[0], args) == -1) {
            perror("execvp"); 
            close(fileno(stdin));
            close(fileno(stdout));
            close(fileno(stderr));
            exit(1);
        }
        LOGP("--------------------------------CHILD RETURNING\n");
        exit(0);
    } 
    else 
    {
        /* I am the parent */
        int status; 
        char* last_token = args[elist_size(list) - 2];
        LOG("Last token is %s\n", last_token);
        if (last_token[strlen(last_token) - 1] == '&') {
            /* Background */
            LOGP("GETTING BACKGROUND JOB IN PARENT\n");
            struct job job_new;
            job_new.pid = child;

            strcpy(job_new.command, "");
            for (int i = 0; i < elist_size(list) - 1; ++i)
            {
                strcat(job_new.command, args[i]);
                strcat(job_new.command, " ");
            }
            job_add(job_new);
        } else {
            /* Foreground */
            int finished_pid = waitpid(child, &status, 0); 
            set_status(status);
            LOG("Child finished executing: %d, pid %d\n", status, finished_pid);
        }
    }
}

bool jellyfish_built_in(struct elist *list) {
    char **comm = elist_get(list, 0);
    if (comm[0] == NULL )
    {
        return true;
    }
    if (!strcmp(*comm, "cd"))
    {
        jellyfish_cd(list);
        return true;
    } 
    else if (!strcmp(*comm, "exit"))
    {
        jellyfish_exit();
        return true;
    }
    else if (!strcmp(*comm, "history"))
    {
        jellyfish_history();
        return true;
    }
    else if (strncmp(*comm, "!", 1) == 0)
    {
        jellyfish_history_execution(list, comm);
        return true;
    }
    else if (!strcmp(*comm, "jobs")){
        jellyfish_jobs();
        return true;
    }
    return false;
}

void jellyfish_process_command(char *command, struct elist *list, bool repeat) {
        
    if (strlen(command) > 0 && ((strncmp(command, "!", 1)) != 0))
    {
        hist_add(command);
        LOG("Adding into history %s\n", command);
    }

    char *next_tok = command;
    char *curr_tok;

    elist_clear(list);
    while ((curr_tok = next_token(&next_tok, " \t\n")) != NULL)
    {
        if (curr_tok[0] == '#') 
        {
            break;
        } 
        LOG("adding: %s\n", curr_tok);
        elist_add(list, &curr_tok); 
    }
    char *null = (char *) 0;
    elist_add(list, &null); 
    LOG("Processed %zu tokens\n", elist_size(list));

    if (elist_size(list) == 1)
    {
        LOGP("Empty command\n");

        return;
    } 
    bool built_in = jellyfish_built_in(list);
    if (built_in == false)
    {
        jellyfish_execute(list);
    } 
    if (repeat == true) {
        free(command);
    }
}

int main(void)
{
    init_ui();
    hist_init(100);
    job_init();
    struct elist *list = elist_create(0, sizeof(char **));

    signal(SIGINT, SIG_IGN);
    signal(SIGCHLD, child_signal_handler);
    while (true) {
        char *command = read_command();
        if (command == NULL) {
            break;
        }
        LOG("Input command: %s\n", command);
        jellyfish_process_command(command, list, false);
    }
    
    destroy_ui();
    job_destroy();
    hist_destroy();
    elist_destroy(list);
    return 0;
}


