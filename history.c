#include <stddef.h>

#include "history.h"
#include "clist.h"
#include <limits.h>
#include <unistd.h>
struct clist *hist;
//struct clist_iterator iterator;
void hist_init(unsigned int limit)
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    // TODO: is it path max???
    // insertion - capacity
    // hist = clist_create(limit, 256);
}

void hist_destroy(void)
{
    // clist_destroy(hist);
}

void hist_add(const char *cmd)
{
    // clist_add(hist, cmd);

}

void hist_print(void)
{
    // When you implement hist_print, be sure to add an fflush(stdout) 
    //at after you have printed your history list. 
    //This makes sure everything gets flushed out 
    //(and most importantly, the test cases that I'm working on will be able to see the full output!)
    // const char *command;
    // while ((command = clist_iterate_rev(hist, &iterator)) != NULL) {
    //     const char e = *((char *) command);
    //     printf("%s\n", e);
    // }
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
    // return clist_get(hist, (size_t)command_number);
    return NULL;
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    // if (hist->insertion == 0) {
        
    // }
    // return hist->insertions - 1;
    return 0;
}

// circular list