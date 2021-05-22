#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 12
#define MAX_WORD_LEN 20
#define MAX_LINE_LEN 100

char words[MAX_WORDS][MAX_WORD_LEN];
int  num_words = 0;


/* Assuming that line != NULL */

void store_the_words(char *line)
{
    char buffer[MAX_LINE_LEN];
    char *t;

    strncpy(buffer, line, MAX_LINE_LEN);
    t = strtok(buffer, " ");
    while (t != NULL) {
        if (num_words >= MAX_WORDS) {
            fprintf(stderr, "Too many words!\n");
            exit(1);    
        }
        strncpy(words[num_words], t, MAX_WORD_LEN);
        num_words++; 

        t = strtok(NULL, " ");
    }
}


int main(int argc, char *argv[]) {
    int i;
    char *line1 = "The quick brown --- fox";
    char *line2 = "jumped over the lazy dog.";

    store_the_words(line1);
    store_the_words(line2);

    for (i = 0; i < num_words; i++) {
        printf("%2d: '%s'\n", i, words[i]);
    }
}
