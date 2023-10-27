/*
 * Skeleton code for CSC 360, Spring 2021,  Assignment #4
 *
 * Prepared by: Michael Zastre (University of Victoria) 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Some compile-time constants.
 */

#define REPLACE_NONE 0
#define REPLACE_FIFO 1
#define REPLACE_LRU  2
#define REPLACE_SECONDCHANCE 3
#define REPLACE_OPTIMAL 4


#define TRUE 1
#define FALSE 0
#define PROGRESS_BAR_WIDTH 60
#define MAX_LINE_LEN 100

struct QNode {
   long frame;
   struct QNode *next;
};

struct Queue {
    struct QNode *head, *tail;
};

/*
 * Some function prototypes to keep the compiler happy.
 */
int setup(void);
int teardown(void);
int output_report(void);
long resolve_address(long, int, struct Queue*);
void error_resolve_address(long, int);
struct QNode* newNode(long);
struct Queue* createQueue(void);
void addEnd(struct Queue*, long);
void removeFront(struct Queue*);
void removeInPlace(struct Queue*, long);


/*
 * Variables used to keep track of the number of memory-system events
 * that are simulated.
 */
int page_faults = 0;
int mem_refs    = 0;
int swap_outs   = 0;
int swap_ins    = 0;


/*
 * Page-table information. You may want to modify this in order to
 * implement schemes such as SECONDCHANCE. However, you are not required
 * to do so.
 */
struct page_table_entry *page_table = NULL;
struct page_table_entry {
    long page_num;
    int dirty;
    int free;
    int ref_bit;
};


/*
 * These global variables will be set in the main() function. The default
 * values here are non-sensical, but it is safer to zero out a variable
 * rather than trust to random data that might be stored in it -- this
 * helps with debugging (i.e., eliminates a possible source of randomness
 * in misbehaving programs).
 */

int size_of_frame = 0;  /* power of 2 */
int size_of_memory = 0; /* number of frames */
int page_replacement_scheme = REPLACE_NONE;


/*  
 * Function that creates a new node and initializes its values 
 */
struct QNode* newNode(long k)
{
    struct QNode *temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->frame = k;
    temp->next = NULL;
    return temp;
}

/*  
 * Function that creates a new queue and initializes its values 
 */
struct Queue* createQueue()
{
    struct Queue *q = (struct Queue*)malloc(sizeof(struct Queue));
    q->head = q->tail = NULL;
    return q;
}

/*  
 * Function that adds a new node to the rear of a queue
 */
void addEnd(struct Queue *q, long k)
{
    struct QNode *temp = newNode(k);
  
    // If queue is empty, then new node is front and rear both
    if (q->head == NULL) {
        q->head = q->tail = temp;
        return;
    }

    // Add the new node at the end of queue and change rear
    q->tail->next = temp;
    q->tail = temp;
}

/*  
 * Function that removes the front node of a queue
 */
void removeFront(struct Queue *q)
{
    if (q->head == NULL){
        return;
    }

    struct QNode *temp = q->head;
  
    q->head = q->head->next;
  
    // If front becomes NULL, then change rear also as NULL
    if (q->head == NULL){
        q->tail = NULL;
    }
  
    free(temp);
}

/*  
 * Function that finds a node within a queue and deletes it from its current position
 */
void removeInPlace(struct Queue *q, long frame)
{
    long val = -1;
    struct QNode *prev = newNode(val);
    struct QNode *temp = newNode(val);
    
    prev = q->head;
    temp = q->head;
    
    // if the node being removed is at the front of the queue
    if(temp != NULL && temp->frame == frame){
        q->head = temp->next;
        free(temp);
        return;
    }

    // search through the queue to find node that is being deleted
    while (temp != NULL && temp->frame != frame) {
        prev = temp;
        temp = temp->next;
    }

    // if the node was not found within the queue do nothing
    if (temp == NULL){
        return;
    }

    // delete the node from the queue
    prev->next = temp->next;

    // if the node being deleted was at the rear of the queue, set tail pointer to new rear
    if(temp->frame == q->tail->frame){
        q->tail = prev;
        free(temp);
        return;
    }

    free(temp);
}


/*
 * Function to convert a logical address into its corresponding 
 * physical address. The value returned by this function is the
 * physical address (or -1 if no physical address can exist for
 * the logical address given the current page-allocation state).
 */

long resolve_address(long logical, int memwrite, struct Queue *frame_q)
{
    int i;
    long page, frame;
    long offset;
    long mask = 0;
    long effective;

    /* Get the page and offset */
    page = (logical >> size_of_frame);

    for (i=0; i<size_of_frame; i++) {
        mask = mask << 1;
        mask |= 1;
    }
    offset = logical & mask;

    /* Find page in the inverted page table. */
    frame = -1;
    for ( i = 0; i < size_of_memory; i++ ) {
        if (!page_table[i].free && page_table[i].page_num == page) {
            frame = i;
            break;
        }
    }

    /* If frame is not -1, then we can successfully resolve the
     * address and return the result. */
    // we go into this loop if we encounter a page hit
    if (frame != -1) {
        
        // set the dirty bit of the particular page
        if(memwrite){
            page_table[frame].dirty = TRUE;
        }
        else{
            page_table[frame].dirty = FALSE;
        }
        
        if(page_replacement_scheme == REPLACE_LRU){
            // place the page at the back of the queue as it is now the most recently used
            removeInPlace(frame_q, frame);
            addEnd(frame_q, frame);

        }
        if(page_replacement_scheme == REPLACE_SECONDCHANCE){
            // set the ref bit as the page now gets its "second chance"
            page_table[frame].ref_bit = TRUE;

            // place page at the back of the queue
            removeInPlace(frame_q, frame);
            addEnd(frame_q, frame);
        }

        effective = (frame << size_of_frame) | offset;
        return effective;
    }


    /* If we reach this point, there was a page fault. Find
     * a free frame. */
    page_faults++;
    swap_ins++;

    for ( i = 0; i < size_of_memory; i++) {
        if (page_table[i].free) {
            frame = i;
            break;
        }
    }

    /* If we found a free frame, then patch up the
     * page table entry and compute the effective
     * address. Otherwise return -1.
     */
    if (frame != -1) {
        // update page table entry info
        page_table[frame].page_num = page;
        page_table[frame].free = FALSE;
        page_table[frame].ref_bit = TRUE;
        page_table[frame].dirty = TRUE;

        // add pages to the queue
        addEnd(frame_q, frame);

        effective = (frame << size_of_frame) | offset;
        return effective;
    } 
    else {
        if (page_replacement_scheme == REPLACE_FIFO) {
            if(frame_q->head != NULL){
                // only if dirty bit is set do you increment swap outs
                if(page_table[frame_q->head->frame].dirty){
                    swap_outs++;
                }

                // update page table entry info
                page_table[frame_q->head->frame].page_num = page;
                page_table[frame_q->head->frame].free = FALSE;
                if(memwrite){
                    page_table[frame_q->head->frame].dirty = TRUE;
                }
                else{
                    page_table[frame_q->head->frame].dirty = FALSE;
                }

                // place new page at the back of the queue and delete the first entry
                addEnd(frame_q, frame_q->head->frame);
                removeFront(frame_q);
            }
        }

        else if (page_replacement_scheme == REPLACE_LRU) {
            // only if dirty bit is set do you increment swap outs
            if(page_table[frame_q->head->frame].dirty){
                swap_outs++;
            }

            // update page table entry info
            page_table[frame_q->head->frame].page_num = page;
            page_table[frame_q->head->frame].free = FALSE;
            if(memwrite){
                page_table[frame_q->head->frame].dirty = TRUE;
            }
            else{
                page_table[frame_q->head->frame].dirty = FALSE;
            }

            // place new page at the back of the queue as it is now the most recently used
            addEnd(frame_q, frame_q->head->frame);
            // and delete the first entry which was the least recently used
            removeFront(frame_q);
        }
        
        else if (page_replacement_scheme == REPLACE_SECONDCHANCE) {
            // if queue head reference bit is set i.e it gets a second chance
            if(frame_q->head != NULL && page_table[frame_q->head->frame].ref_bit){
                long val = -1;

                //node that will store the first page which does not have its ref bit set
                struct QNode *curr = newNode(val);
                //node that will store the last page which had its ref bit set
                struct QNode *last_refbit_node = newNode(val);
                
                curr = frame_q->head;

                //assuming there are multiple contiguous pages with ref bit set
                while(curr != NULL && page_table[curr->frame].ref_bit){
                    page_table[curr->frame].ref_bit = FALSE;
                    last_refbit_node = curr;
                    curr = curr->next;
                }

                // going into this loop means every page in queue had its ref bit set
                // meaning now, all the ref bits in the queue have been set to 0
                if(curr == NULL){
                    last_refbit_node = NULL;
                    curr = frame_q->head;
                }

                /* create a temporary queue which has all the entries following and including the first
                entry that did not have its ref bit set */
                struct Queue *temp_q = createQueue();
                while(curr != NULL){
                    addEnd(temp_q, curr->frame);
                    curr = curr->next;
                }

                //perform regular FIFO on temp queue
                if(page_table[temp_q->head->frame].dirty){
                    swap_outs++;
                }

                page_table[temp_q->head->frame].page_num = page;
                page_table[temp_q->head->frame].free = FALSE;
                page_table[temp_q->head->frame].ref_bit = TRUE;
                if(memwrite){
                    page_table[temp_q->head->frame].dirty = TRUE;
                }
                else{
                    page_table[temp_q->head->frame].dirty = FALSE;
                }

                addEnd(temp_q, temp_q->head->frame);
                removeFront(temp_q);

                //append temp queue back to regular queue
                if(last_refbit_node == NULL){
                    frame_q = temp_q;
                }
                else{
                    //to free up memory keep track of the rest of the original queue which is being discarded
                    curr = last_refbit_node->next;

                    //append
                    last_refbit_node->next = temp_q->head;

                    //set the new tail of the queue
                    while(last_refbit_node->next != NULL){
                        last_refbit_node = last_refbit_node->next;
                    }
                    frame_q->tail = last_refbit_node;
                }

                // free up memory
                if(last_refbit_node == NULL){
                    free(curr);
                    free(last_refbit_node);
                }
                else{
                    last_refbit_node = curr;
                    while (curr->next != NULL){
                        curr = last_refbit_node->next;
                        free(last_refbit_node);
                        last_refbit_node = curr;
                    }
                    free(curr);
                }
                
                free(temp_q);

            }

            // if queue head reference bit is not set i.e it does not get a second chance
            else if(frame_q->head != NULL){
                
                // regular FIFO
                if(page_table[frame_q->head->frame].dirty){
                    swap_outs++;
                }

                page_table[frame_q->head->frame].page_num = page;
                page_table[frame_q->head->frame].free = FALSE;
                page_table[frame_q->head->frame].ref_bit = TRUE;
                if(memwrite){
                    page_table[frame_q->head->frame].dirty = TRUE;
                }
                else{
                    page_table[frame_q->head->frame].dirty = FALSE;
                }

                addEnd(frame_q, frame_q->head->frame);
                removeFront(frame_q);
            }

        }
        return 1;
    }
}



/*
 * Super-simple progress bar.
 */
void display_progress(int percent)
{
    int to_date = PROGRESS_BAR_WIDTH * percent / 100;
    static int last_to_date = 0;
    int i;

    if (last_to_date < to_date) {
        last_to_date = to_date;
    } else {
        return;
    }

    printf("Progress [");
    for (i=0; i<to_date; i++) {
        printf(".");
    }
    for (; i<PROGRESS_BAR_WIDTH; i++) {
        printf(" ");
    }
    printf("] %3d%%", percent);
    printf("\r");
    fflush(stdout);
}


int setup()
{
    int i;

    page_table = (struct page_table_entry *)malloc(
        sizeof(struct page_table_entry) * size_of_memory
    );

    if (page_table == NULL) {
        fprintf(stderr,
            "Simulator error: cannot allocate memory for page table.\n");
        exit(1);
    }

    for (i=0; i<size_of_memory; i++) {
        page_table[i].free = TRUE;
        page_table[i].ref_bit = FALSE;
    }

    return -1;
}


int teardown()
{

    return -1;
}


void error_resolve_address(long a, int l)
{
    fprintf(stderr, "\n");
    fprintf(stderr, 
        "Simulator error: cannot resolve address 0x%lx at line %d\n",
        a, l
    );
    exit(1);
}


int output_report()
{
    printf("\n");
    printf("Memory references: %d\n", mem_refs);
    printf("Page faults: %d\n", page_faults);
    printf("Swap ins: %d\n", swap_ins);
    printf("Swap outs: %d\n", swap_outs);

    return -1;
}


int main(int argc, char **argv)
{
    /* For working with command-line arguments. */
    int i;
    char *s;

    /* For working with input file. */
    FILE *infile = NULL;
    char *infile_name = NULL;
    struct stat infile_stat;
    int  line_num = 0;
    int infile_size = 0;

    /* For processing each individual line in the input file. */
    char buffer[MAX_LINE_LEN];
    long addr;
    char addr_type;
    int  is_write;

    /* For making visible the work being done by the simulator. */
    int show_progress = FALSE;

    /* Creating the queue to be used to imlpement page replacement techniques. */
    struct Queue *pageframe_q = createQueue();

    /* Process the command-line parameters. Note that the
     * REPLACE_OPTIMAL scheme is not required for A#3.
     */
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "--replace=", 9) == 0) {
            s = strstr(argv[i], "=") + 1;
            if (strcmp(s, "fifo") == 0) {
                page_replacement_scheme = REPLACE_FIFO;
            } else if (strcmp(s, "lru") == 0) {
                page_replacement_scheme = REPLACE_LRU;
            } else if (strcmp(s, "secondchance") == 0) {
                page_replacement_scheme = REPLACE_SECONDCHANCE;
            } else if (strcmp(s, "optimal") == 0) {
                page_replacement_scheme = REPLACE_OPTIMAL;
            } else {
                page_replacement_scheme = REPLACE_NONE;
            }
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            infile_name = strstr(argv[i], "=") + 1;
        } else if (strncmp(argv[i], "--framesize=", 12) == 0) {
            s = strstr(argv[i], "=") + 1;
            size_of_frame = atoi(s);
        } else if (strncmp(argv[i], "--numframes=", 12) == 0) {
            s = strstr(argv[i], "=") + 1;
            size_of_memory = atoi(s);
        } else if (strcmp(argv[i], "--progress") == 0) {
            show_progress = TRUE;
        }
    }

    if (infile_name == NULL) {
        infile = stdin;
    } else if (stat(infile_name, &infile_stat) == 0) {
        infile_size = (int)(infile_stat.st_size);
        /* If this fails, infile will be null */
        infile = fopen(infile_name, "r");  
    }


    if (page_replacement_scheme == REPLACE_NONE ||
        size_of_frame <= 0 ||
        size_of_memory <= 0 ||
        infile == NULL)
    {
        fprintf(stderr, 
            "usage: %s --framesize=<m> --numframes=<n>", argv[0]);
        fprintf(stderr, 
            " --replace={fifo|lru|optimal} [--file=<filename>]\n");
        exit(1);
    }


    setup();

    while (fgets(buffer, MAX_LINE_LEN-1, infile)) {
        line_num++;
        if (strstr(buffer, ":")) {
            sscanf(buffer, "%c: %lx", &addr_type, &addr);
            if (addr_type == 'W') {
                is_write = TRUE;
            } else {
                is_write = FALSE;
            }

            if (resolve_address(addr, is_write, pageframe_q) == -1) {
                error_resolve_address(addr, line_num);
            }
            mem_refs++;
        } 

        if (show_progress) {
            display_progress(ftell(infile) * 100 / infile_size);
        }
    }
    

    teardown();
    output_report();

    fclose(infile);

    exit(0);
}
