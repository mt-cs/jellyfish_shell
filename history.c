#include <stddef.h>

#include "history.h"

void hist_init(unsigned int limit)
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
}

void hist_destroy(void)
{

}

void hist_add(const char *cmd)
{

}

void hist_print(void)
{
    // When you implement hist_print, be sure to add an fflush(stdout) 
    //at after you have printed your history list. 
    //This makes sure everything gets flushed out 
    //(and most importantly, the test cases that I'm working on will be able to see the full output!)
}

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    return NULL;
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    return 0;
}

// circular list