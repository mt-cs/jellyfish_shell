#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "history.h"
#include "logger.h"
#include "ui.h"

static const char *good_str = "😌";
static const char *bad_str  = "😱";

static int readline_init(void);
static bool scripting = false;

void init_ui(void)
{
    LOGP("Initializing UI...\n");

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");
	if (isatty(STDIN_FILENO) == false) {
	        LOGP("Entering interactive mode\n");
	        scripting = true;
	    } 
    rl_startup_hook = readline_init;
}

void destroy_ui(void)
{
    // TODO cleanup code, if necessary
}

char *prompt_line(void)
{
    const char *status = prompt_status() ? bad_str : good_str; 
    // because, conventionally, programs return 0 when they ran successfully.

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
        return "unknown_user"; //TODO: warning: unused variable ‘i’ [-Wunused-variable] i = 0
    }

    return username;
}

char *prompt_hostname(void)
{
    char host_name[HOST_NAME_MAX + 1];
    //host_name[HOST_NAME_MAX] = '\0';
    int h = gethostname(host_name, sizeof(host_name));
    char *host = host_name;
    if (h == -1) {
        return "unknown_hostname";
    }
    return host;
}

char *prompt_cwd(void)
{
    return "/unknown/path";
}

int prompt_status(void)
{
    return -1;
}

unsigned int prompt_cmd_num(void)
{
    return 0;
}

char *read_command(void)
{
	if (scripting == false) {
		char *prompt = prompt_line();
	    char *command = readline(prompt);
	    free(prompt);
	    return command;
	} else {
		// do what we did in class with getline
		// getline includes the trailing \n (newline) character
        // trim off the \n manually on scripting mode
		// but readline DOES NOT!
        /*Since the readline library we’re using for the shell UI is intended for interactive use, 
        you will need to switch to a traditional input reading function 
        such as getline when operating in scripting mode.
        When implementing scripting mode, you will likely need 
        to close stdin on the child process if your call to exec() fails. 
        This prevents infinite loops.*/
        char *line= NULL;
        size_t line_sz = 0;
        ssize_t read_sz = getline(&line, &line_sz, stdin);
        if (read_sz == -1) {
            perror("getline");
            return NULL;
        }
        line[read_sz - 1] = '\0';
        LOG("got a command: %s", line);
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
    rl_replace_line("User pressed 'up' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

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

    // TODO: step forward through the history (assuming we have stepped back
    // previously). Going past the most recent history command blanks out the
    // command line to allow the user to type a new command.

	// Create a global variable to tracl
    return 0;
}
