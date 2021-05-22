#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int i;
    char *filename = NULL;

    for (i = 0; i < argc; i++) {
        printf("%d: %s\n", i, argv[i]);

        if (strcmp(argv[i], "-e") == 0) {
            filename = argv[i+1];
            i++;
            /* Why does the argument after the "-e"
             * no longer print???!! */
        }
    }

    if (filename != NULL) {
        printf("The filename was: %s\n", filename);
    } else {
        printf("No soup for you\n");
    }
}
