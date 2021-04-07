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

#include "logger.h"

#define MAX_COMMAND_LENGTH (256)
#define MAX_HISTORY_CAPACITY (100)
static struct clist *hist;
static struct clist_iterator iterator;

void hist_init(unsigned int limit)
{
    // set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
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
    void *command;
    size_t index;
    iterator.idx = 0;
    while ((command = clist_iterate_rev(hist, &iterator)) != NULL) {
        if (hist->insertions < MAX_HISTORY_CAPACITY) {
            index = iterator.idx;
        } else {
            index = hist->insertions - MAX_HISTORY_CAPACITY + iterator.idx;
        }
        printf("%zu %s\n", index, (char *)command);
    }
}

const char *hist_search_prefix(char *prefix)
{
    if (hist->insertions == 0) {
        return NULL;
    }
    void *command;
    //iterator.idx = (hist->insertions - 1) % MAX_HISTORY_CAPACITY;
    iterator.idx = 0;
    while ((command = clist_iterate(hist, &iterator)) != NULL) 
    {
        if (strlen(command) < strlen(prefix)) {
            continue;
        } else {
            if (strncmp(prefix, command, strlen(prefix)) == 0) {
                return command;
            }    
        }
    }
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // Retrieves a particular command number. Return NULL if no match found.
    return clist_get(hist, (size_t)command_number);
}

unsigned int hist_last_cnum(void)
{
    // Retrieve the most recent command number.
    if (hist->insertions == 0) {
        LOGP("Insertion error!");
        exit(-1);
    }
    return hist->insertions - 1;
}