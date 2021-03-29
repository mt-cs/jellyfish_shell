#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int main(void) {
    char host_name[HOST_NAME_MAX + 1];
    gethostname(host_name, sizeof(host_name));
    printf("hostname: %s\n", host_name);

    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        perror("getcwd() error");
    else
        printf("current working directory is: %s\n", cwd);
    
    return EXIT_SUCCESS;
}