#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

int main(void) {
  char host_name[HOST_NAME_MAX + 1];
    gethostname(host_name, sizeof(host_name));

    //return *host_name;

  printf("hostname: %s\n", host_name);

  return EXIT_SUCCESS;
}