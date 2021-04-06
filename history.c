/**
 * @file
 */

#include <stddef.h>
#include "history.h"
#include "clist.h"
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int MAX_COMMAND_LENGTH = 256;
static struct clist *hist;
static struct clist_iterator iterator;

void hist_init(unsigned int limit)
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    // insertion - capacity
    iterator = clist_create_iter();
    hist = clist_create(limit, (sizeof(char) * MAX_COMMAND_LENGTH));
}

void hist_destroy(void)
{
    clist_destroy(hist);
}

void hist_add(const char *cmd)
{
    //clist_add(hist, cmd);
    char *target = strdup(cmd);
    clist_add(hist, target);
    free(target);
}

void hist_print(void)
{
    // When you implement hist_print, be sure to add an fflush(stdout) 
    //at after you have printed your history list. 
    //This makes sure everything gets flushed out 
    //(and most importantly, the test cases that I'm working on will be able to see the full output!)
    void *command;
    long unsigned int index;
    
    while ((command = clist_iterate_rev(hist, &iterator)) != NULL) {
        if (hist->insertions < 100) {
            index = iterator.idx;
        } else {
            index = hist->insertions - 100 + iterator.idx;
        }
        printf("%zu %s\n", index, (char *)command);
    }
}

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    // const char *command;
    // while ((command = clist_iterate_rev(hist, &iterator)) != NULL) 
    // {
    //     const char e = *((const char *) command);
    //     if (strlen(e) > strlen(prefix)) {
    //         continue;
    //     } else {
    //         if (strncmp(prefix, e, strlen(prefix)) == 0) {
    //             return e;
    //         }    
    //     }
    // }
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    return clist_get(hist, (size_t)command_number);
    //return NULL;
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    if (hist->insertions == 0) {
        return 0;
    }
    return hist->insertions - 1;
}