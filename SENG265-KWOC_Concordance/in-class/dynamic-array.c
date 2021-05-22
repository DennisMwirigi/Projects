/*
 * From pages 41 to 44, "The Practice of Programming" by Pike
 * and Kernighan, Addison-Wesley, 1999.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NAME_LEN  50


typedef struct Nameval Nameval;
struct Nameval {
    char name[MAX_NAME_LEN];
    int  value;
};

struct NVtab {
    int    nval;
    int    max;
    Nameval *nameval;
} nvtab;

enum {
    NVINIT = 1,
    NVGROW = 2
};


/* addname: add new name and value to `nvtab`. */
int addnameval(char *name, int val)
{
    Nameval *nvp;
    
    /* initial table is empty? */
    if (nvtab.nameval == NULL) { 
        nvtab.nameval = (Nameval *)malloc(NVINIT * sizeof(Nameval));
        if (nvtab.nameval == NULL) {
            /* Something went horribly wrong ... */
            return -1;
        }
        nvtab.max = NVINIT;
        nvtab.nval = 0;
    }
    /* table has no space available? if so, grow. */
    else if (nvtab.nval >= nvtab.max) {
        nvp = (Nameval *) realloc(nvtab.nameval,
            (NVGROW * nvtab.max) * sizeof(Nameval));
        if (nvp == NULL) {
            return -1;
        }
        nvtab.max *= NVGROW;
        nvtab.nameval = nvp;
    }

    /* If we arrive at this point, we have enough room
     * in the array to add at item. */
    strncpy(nvtab.nameval[nvtab.nval].name,name, MAX_NAME_LEN);
    nvtab.nameval[nvtab.nval].value = val;
    return nvtab.nval++;
}


/* delname: remove *first* matching nameval from nvtab */
int delname(char *name)
{
    int i;

    for (i = 0; i < nvtab.nval; i++) {
        if (strcmp(nvtab.nameval[i].name, name) == 0) {
            memmove(nvtab.nameval+i, nvtab.nameval+i+1,
                (nvtab.nval-(i+1)) * sizeof(Nameval));
            nvtab.nval--;
            return 1;
        }
    }

    return 0;
}


void dump_nvtab()
{
    int i = 0;

    printf("----------------------\n");
    printf("max:  %d\n", nvtab.max);
    printf("nval: %d\n", nvtab.nval);
    printf("location of nameval: %p\n", nvtab.nameval);
    printf("----------------------\n");

    for (i = 0; i < nvtab.nval; i++) {
        printf("%3d: %s\n", i, nvtab.nameval[i].name);
    }
}


int main(int argc, char *argv[]) {
    char *dummy_list[] = {"the", "quick", "brown",
        "fox", "jumped", "over", "the", "lazy",
        "dog"};
    int  dummy_list_len = 9;
    int  i;

    /* Initialize the `nvtab` table. */
    nvtab.nameval = NULL;
    nvtab.nval = nvtab.max = 0;

    dump_nvtab();

    for (i = 0; i < dummy_list_len; i++) {
        printf("Inserting `%s`\n", dummy_list[i]);
        addnameval(dummy_list[i], 0);
        dump_nvtab();
    }


    printf("Deleting '%s': %d\n", dummy_list[1],
        delname(dummy_list[1]));
    dump_nvtab();

    printf("Deleting '%s': %d\n", dummy_list[1],
        delname(dummy_list[1]));
    dump_nvtab();

    printf("Deleting '%s': %d\n", dummy_list[5],
        delname(dummy_list[5]));
    dump_nvtab();


    char *dummy_list2[] = {"shall", "i", "compare", "thee",
        "too", "a", "summer's", "day?"};
    int dummy_list2_len = 8;

    for (i = 0; i < dummy_list2_len; i++) {
        printf("Inserting `%s`\n", dummy_list2[i]);
        addnameval(dummy_list2[i], 0);
        dump_nvtab();
    }

    printf("Deleting '%s': %d\n", dummy_list[4],
        delname(dummy_list[4]));
    dump_nvtab();

    free(nvtab.nameval);

    exit(0);
}
