#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 100

/* Assuming haystack is not NULL, that needle is not NULL. */

int num_occurrences(char *haystack, char *needle)
{
    char buffer[MAX_LINE_LEN];
    char *t;
    int  num_occur = 0;

    strncpy(buffer, haystack, MAX_LINE_LEN);

    t = strtok(buffer, " ");
    while (t != NULL) {
        if (strcmp(needle, t) == 0) {
            num_occur++;
        }
        t = strtok(NULL, " ");
    }

    return (num_occur);
}


/* Assuming that argv[1] is not null and that argv[2] is not
 * null -- which are dangerous assumptions. */

int main(int argc, char *argv[]) {
    FILE *infile;
    char buffer[MAX_LINE_LEN];
    int  num_occur = 0;
    char *search_word;

    if (argc < 3) {
        fprintf(stderr, "Need two arguments\n");
        exit(1);
    }

    infile = fopen(argv[1], "r");
    if (infile == NULL) {
        fprintf(stderr, "Problems opening %s for reading\n",
            argv[1]);
        exit(1);
    }

    search_word = argv[2];

    while (fgets(buffer, MAX_LINE_LEN, infile) != NULL) {
        num_occur = num_occur + num_occurrences(buffer, search_word);
    }

    printf("%d\n", num_occur);

    fclose(infile);
}
