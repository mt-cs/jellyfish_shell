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

int status_num;
int com_number;
static const char *good_str = "😊";
static const char *bad_str  = "🤮";

static int readline_init(void);
static bool scripting = false;
static ssize_t read_sz;
static char *line= NULL;
static unsigned int key_command;

void init_ui(void)
{
    LOGP("Initializing UI...\n");

    char *locale = setlocale(LC_ALL, "en_US.UTF-8"); //set or query the program's current locale
    // LC_ALL for all of the locale.
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!"); // If locale is NULL, the current locale is only queried, not modified.
	if (isatty(STDIN_FILENO) == false) { // test whether a file descriptor refers to a terminal
	        LOGP("Entering interactive mode\n");
	        scripting = true;
	    } 
    rl_startup_hook = readline_init;
}

void destroy_ui(void)
{
    free(line);
}

char *prompt_line(void)
{
    const char *status = prompt_status() ? bad_str : good_str;
    char cmd_num[25];
    snprintf(cmd_num, 25, "%d", prompt_cmd_num());

    char *user = prompt_username();
    char *host = prompt_hostname();
    char *cwd = prompt_cwd();

    char *format_str = ">>-[%s]-[%s]-[%s@%s:%s]-> ";

    size_t prompt_sz
        = strlen(format_str)
        + strlen(status)
        + strlen(cmd_num)
        + strlen(user)
        + strlen(host)
        + strlen(cwd)
        + 1;

    char *prompt_str =  malloc(sizeof(char) * prompt_sz);

    snprintf(prompt_str, prompt_sz, format_str,
            status,
            cmd_num,
            user,
            host,
            cwd);
   
    return prompt_str;
}

char *prompt_username(void)
{
    char *username = getlogin();
    if (username == NULL) {
        return "unknown_user"; 
    }

    return username;
}

char *prompt_hostname(void)
{
    static char host_name[HOST_NAME_MAX + 1] = { 0 };
    gethostname(host_name, sizeof(host_name));
    return host_name;
}

char *prompt_cwd(void)
{
    static char *cwd = NULL;
    if (cwd != NULL) {
        free(cwd);
        LOGP("free cwd\n");
    } 
    cwd = getcwd(NULL, 0);
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    LOG("Current user dir: %s\n", homedir);
    
    if (strncmp(homedir, cwd, strlen(homedir)) == 0) {
        char *new_cwd = cwd + strlen(homedir) - 1;
        new_cwd[0] = '~';
        LOG("New cwd: %s\n", new_cwd);
        return new_cwd; 
    }
    return cwd; 
}

int set_status(int status) {
    status_num = status;
    LOG("status: %d\n", status_num);
    return 0;
}
 
int prompt_status(void)
{
    return status_num;
}

int prompt_cmd_num(void)
{
    return com_number;
}

char *read_command(void)
{
    key_command = - 1;
	if (scripting == false) {
        com_number++;
		char *prompt = prompt_line();
	    char *command = readline(prompt);
	    free(prompt);
	    return command;
	} else {
        static size_t line_sz = 0;
        read_sz = getline(&line, &line_sz, stdin);
        if (read_sz == -1) {
            perror("getline");
            return NULL;
        }
        line[read_sz - 1] = '\0';
        LOG("got a command: %s\n", line);
		return line;
	}
}

int readline_init(void)
{
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    return 0;
}

int key_up(int count, int key)
{
    if (key_command == -1) {
        key_command = hist_last_cnum() + 1;
    }
    key_command--;
    const char *key_search = hist_search_cnum(key_command);

    /* Modify the command entry text: */
    if (key_search != NULL) {
        rl_replace_line(key_search, 1); 
    }
    
    /* Move the cursor to the end of the line: */
    rl_point = rl_end;
    return 0;
}

int key_down(int count, int key)
{
    if (key_command == -1) {
        key_command = hist_last_cnum() + 1;
    }
    key_command++;
    const char *key_search = hist_search_cnum(key_command);
    /* Modify the command entry text: */
    if (key_search != NULL) {
        rl_replace_line(key_search, 1); 
    }
    /* Move the cursor to the end of the line: */
    rl_point = rl_end;
    return 0;
}