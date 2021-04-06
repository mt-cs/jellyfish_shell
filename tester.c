/**
 * @file
 */

#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include "history.h"
#include "logger.h"
#include "ui.h"
#include <sys/types.h>
#include <pwd.h>

int main()
{
    hist_init(100);
    char *command = "echo hello world";
    hist_add(command);

    char *cmd = "ls -l junting hi";
    hist_add(cmd);

    char *cmd2 = "cat barbora barbora barbora";
    hist_add(cmd2);

    char *cmd3 = "cat hello";
    hist_add(cmd3);
    hist_print();

    const char *hist_cnum = hist_search_cnum(1);
   // const char ie = *((const char *) hist);
    printf("second entered command is:  --> %s\n", hist_cnum);

   //printf("%d\n", hist->insertions);
    unsigned int res = hist_last_cnum();
    printf("%d\n", res);

    hist_destroy();
    return 0;
}