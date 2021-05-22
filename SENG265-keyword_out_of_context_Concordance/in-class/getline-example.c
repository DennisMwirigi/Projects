#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


int main(void)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;   /* size_t is really just an unsigned int */
    ssize_t read;     /* ssize_t is where a function may return a size or a
                       * negative number. */

    fp = fopen("/etc/motd", "r");
    if (fp == NULL) {
        exit(1);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
    }

    if (line) {
        free(line);
    }
    exit(0);
}

