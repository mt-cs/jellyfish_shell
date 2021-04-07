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
static const char *good_str = "😌";
static const char *bad_str  = "🤮";

static int readline_init(void);
static bool scripting = false;
static ssize_t read_sz;
static char *line= NULL;

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
    // because, conventionally, programs return 0 when they ran successfully.
    char cmd_num[25];
    snprintf(cmd_num, 25, "%ud", prompt_cmd_num());

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
    //LOG("Current user dir: %s\n", homedir);
    
    if (strncmp(homedir, cwd, strlen(homedir)) == 0) {
        char *new_cwd = cwd + strlen(homedir) - 1;
        new_cwd[0] = '~';
        //LOG("New cwd: %s\n", new_cwd);
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

unsigned int prompt_cmd_num(void)
{
    return com_number;
}

char *read_command(void)
{
	if (scripting == false) {
        com_number++;
		char *prompt = prompt_line();
	    char *command = readline(prompt);
	    free(prompt);
	    return command;
	} else {
        //static char *line= NULL;
        size_t line_sz = 0;
        read_sz = getline(&line, &line_sz, stdin);
        //ssize_t read_sz = getline(&line, &line_sz, stdin);
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
    /* Modify the command entry text: */
    char *command_key = "User pressed 'up' key";
    rl_replace_line(command_key, 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;
    // iter
    // TODO: step back through the history until no more history entries are
    // left. Once the end of the history is reached, stop updating the command
    // line.

    return 0;
}

int key_down(int count, int key) // not so useful, count probably 1
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'down' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;
    // iter rev
    // TODO: step forward through the history (assuming we have stepped back
    // previously). Going past the most recent history command blanks out the
    // command line to allow the user to type a new command.

	// Create a global variable to track
    return 0;
}