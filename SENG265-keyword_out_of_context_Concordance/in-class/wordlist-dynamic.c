/*
 * With slight tweak to include prototype for compare function
 * used by qsort.
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_LINE_LEN 80
#define MAX_WORDS 100

char *words[MAX_WORDS];
int num_words = 0;

/* prototypes */
void add_words(char *);
void dump_words(void);
int  strcmp_wrapper(const void *, const void *);


void add_words(char *line) {
    char *t;

    t = strtok(line, " ");
    while (t) {
        words[num_words] = (char *)malloc(sizeof(char) * (strlen(t) + 1));
        if (words[num_words] == NULL) {
            fprintf(stderr, "Problems at the ranch.");
            exit(1);
        }
        strncpy(words[num_words], t, strlen(t) + 1);
        num_words++;
        t = strtok(NULL, " ");
    }
}

void dump_words() {
    int i;

    printf("WORD DUMP:\n");
    for (i = 0; i < num_words; i++) {
        printf("%s\n", words[i]);
    }
}

 
int strcmp_wrapper(const void *a, const void *b) {
    char **sa = (char **)a;
    char **sb = (char **)b;

    return(strcmp(*sa, *sb));
}


int main() {
    char input[MAX_LINE_LEN];

    while (fgets(input, MAX_LINE_LEN, stdin))  {
        input[strlen(input)-1] = '\0';
        add_words(input);
    }

    printf("BEFORE SORT:\n");
    dump_words(); 
    qsort(words, num_words, sizeof(char *),
        strcmp_wrapper);

    printf("\nAFTER SORT:\n");
    dump_words(); 
} 
