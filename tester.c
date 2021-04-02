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

char *prompt_cwd(void)
{
    static char *cwd = NULL;
    if (cwd != NULL) {
        free(cwd);
        LOGP("free cwd\n");
    } 
    cwd = getcwd(NULL, 0);
    //cwd = getcwd(NULL, 0);
    /* if the CWD is the user’s home directory, then the entire path is replaced with ~. 
    Subdirectories under the home directory are prefixed with ~; 
    if I am in /home/mmalensek/test, the prompt will show ~/test*/
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    LOG("Current user dir: %s\n", homedir);
    
    if (strncmp(homedir, cwd, strlen(homedir)) == 0) {
        cwd = cwd + strlen(homedir) - 1;
        cwd[0] = '~';
        LOG("New cwd: %s\n", cwd);
    }
    return cwd; 
}

int main()
{
    char *cwd = prompt_cwd();
    printf(cwd);
    return 0;
}