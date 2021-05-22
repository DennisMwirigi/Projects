#include <stdio.h>

#define MAXSIZE 5
#define INITVALUE 0xffff
 
int numbers[MAXSIZE];
int more_numbers[MAXSIZE];

int main() {
    int i;
    int m, n;
    int *pm, *pn;

    for (i = 0; i < MAXSIZE; i++) {
        numbers[i] = INITVALUE;
        more_numbers[i] = INITVALUE;
    }

    printf("Before:\n");
    for (i = 0; i < MAXSIZE; i++) {
        printf("numbers[%d], at address %p = %x\n", 
            i, &numbers[i], numbers[i]);
    }
    for (i = 0; i < MAXSIZE; i++) {
        printf("more_numbers[%d], at address %p = %x\n", 
            i, &more_numbers[i], more_numbers[i]);
    }

    printf("\n");
    printf("Start of numbers[]: %p\n", numbers);
    printf("Start of more_numbers[]: %p\n", more_numbers);


    pm = &more_numbers[2];
    pn = &more_numbers[8]; /* Outside of bounds of more_numbers! */

    printf("\n");
    printf("pm = %p\n", pm);
    printf("pn = %p\n", pn);

    /* Now assign via pointer reference, rather than via
     * array indexing.
     */

    *pm = 0x1010;
    *pn = 0xcccc;


    printf("\n");
    printf("After:\n");
    for (i = 0; i < MAXSIZE; i++) {
        printf("numbers[%d], at address %p = %x\n", 
            i, &numbers[i], numbers[i]);
    }
    for (i = 0; i < MAXSIZE; i++) {
        printf("more_numbers[%d], at address %p = %x\n", 
            i, &more_numbers[i], more_numbers[i]);
    }

}
