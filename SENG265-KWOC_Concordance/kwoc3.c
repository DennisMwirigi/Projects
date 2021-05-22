/*
 * kwoc3.c
 *
 * Starter file provided to students for Assignment #3, SENG 265,
 * Spring 2020,
 * Edited by Dennis Arimi, V00908180
 */
#define _GNU_SOURCE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include "emalloc.h"
#include "listy.h"

#define MAX_WORD_LENGTH 40
#define MAX_LINE_LENGTH 100

typedef struct Lines Lines;
struct Lines {
    char line[MAX_LINE_LENGTH];
};

typedef struct File {
    int    line_num;
    int    max;
    Lines *line;
} File;

enum {
    INIT = 8,
    GROW = 2
};

void get_filenames(char **input_name, char **exception_name, int argc, char *argv[]);
void get_store_tokenize_lines(File *file, char *name, node_t **toks_head);
int store_lines(File *file, char *curr_line);
void tokenize(node_t **toks_head, char *curr_line);
void sort(node_t **toks_head);
int strcmp_wrapper(const void *a, const void *b);
void remove_duplicates(node_t **toks_head);
void remove_exceptions(node_t **in_toks_head, node_t **ex_toks_head);
node_t *delitem (node_t *listp, char *word);
void output(File *input, node_t **keywords);
void word_in_line(char *keyword, char line[MAX_LINE_LENGTH], int *word_occurences);
void free_list(node_t **words);

int main(int argc, char *argv[]){
    //initialising vars to store file names
    char *exception_file = NULL;
    char *input_file = NULL;

    //instances of structs that store dynamic arrays
    File input;
    File exception;

    /*initialise struct instances*/
    //for input
    input.line_num = 0;
    input.max = 0;
    input.line = NULL;

    //for exception
    exception.line_num = 0;
    exception.max = 0;
    exception.line = NULL;

    //initialising list heads
    node_t *in_toks_head = NULL;
    node_t *ex_toks_head = NULL;

    get_filenames(&input_file, &exception_file, argc, argv);

    if(input_file == NULL){
    	fprintf(stderr, "Please provide a filename\n");
    	exit(1);
    } else if(exception_file == NULL) {
        get_store_tokenize_lines(&input, input_file, &in_toks_head);
        if(input.line == NULL){
            exit(0);
        }
        sort(&in_toks_head);
        remove_duplicates(&in_toks_head);
        output(&input, &in_toks_head);

        free(input.line);
        free_list(&in_toks_head);
    } else{
        get_store_tokenize_lines(&input, input_file, &in_toks_head);
        get_store_tokenize_lines(&exception, exception_file, &ex_toks_head);
        sort(&in_toks_head);
        remove_duplicates(&in_toks_head);
        remove_exceptions(&in_toks_head, &ex_toks_head);
        output(&input, &in_toks_head);

        free(input.line);
        free(exception.line);
        free_list(&in_toks_head);
    }
}

//get filenames from command line arguments
void get_filenames(char **input_name, char **exception_name, int argc, char *argv[]){
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i+1 <= argc) {
            *exception_name = argv[++i];
        } else {
            *input_name = argv[i];
        }
    }
}

/* Function that:
 * reads into specified file line by line
 * stores each line into a dynamic array
 * tokenizes each line and stores tokens in a linked list
 */
void get_store_tokenize_lines(File *file, char *name, node_t **toks_head){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;

    fp = fopen(name, "r");

    while ((read = getline(&line, &len, fp)) != -1) {
        store_lines(file, line);
        tokenize(toks_head, line);
    }
        
    if (line) {
        free(line);
    }

    fclose(fp);
}

//store the file lines in a dynamic array
int store_lines(File *file, char *curr_line){
    Lines *lp;

    //if initial table is empty
    if (file->line == NULL) {
        file->line = (Lines *)malloc(INIT * sizeof(Lines));
        file->max = INIT;
        file->line_num = 0;
    }
    //table has no space available? if so, grow.
    else if (file->line_num >= file->max) {
        lp = (Lines *) realloc(file->line,
            (GROW * file->max) * sizeof(Lines));

        file->max *= GROW;
        file->line = lp;
    }

    //we should now have enough space to add a line
    strncpy(file->line[file->line_num].line, curr_line, MAX_LINE_LENGTH);
    
    return file->line_num++;
}

//tokenize lines storing each token (made lowercase) in a linked list
void tokenize(node_t **toks_head, char *curr_line){
    node_t *temp_node = NULL;
    char *t;
	char temp_line[MAX_LINE_LENGTH];
    char token_temp[MAX_WORD_LENGTH];

    strncpy(temp_line, curr_line, MAX_LINE_LENGTH);

    t = strtok(temp_line, " \n");
	while (t != NULL){
        strncpy(token_temp, t, MAX_WORD_LENGTH);
        for(int i=0 ; i<strlen(token_temp); i++){
            token_temp[i] = tolower(token_temp[i]);
        }
        temp_node = new_node(token_temp);
        *toks_head = add_end(*toks_head, temp_node);
		t = strtok(NULL, " \n");
	}
}

void sort(node_t **toks_head){
    int num_keywords = 1;
    node_t *curr_node;

    for(curr_node = *toks_head; curr_node->next != NULL; curr_node = curr_node->next){
        num_keywords += 1;
    }

    char tosort[num_keywords][MAX_WORD_LENGTH];

    curr_node = *toks_head;
    for(int i=0; i<num_keywords; i++){
        strncpy(tosort[i], curr_node->text, MAX_WORD_LENGTH);
        curr_node = curr_node->next;
    }
    
    qsort(tosort, num_keywords, MAX_WORD_LENGTH*sizeof(char), strcmp_wrapper);

    //placing the sorted elements back into the list
    *toks_head = NULL;
    for(int j=0; j<num_keywords; j++){
        curr_node = new_node(tosort[j]);
        *toks_head = add_end(*toks_head, curr_node);
    }
}

//code from lecture
int strcmp_wrapper(const void *a, const void *b) {
    char *sa = (char *)a;
    char *sb = (char *)b;

    return(strcmp(sa, sb));
}

//code from GeeksforGeeks
void remove_duplicates(node_t **toks_head){
    node_t *curr_node = *toks_head;
    node_t *after_duplicate;

    while (curr_node->next != NULL)  
    { 
       if (strncmp(curr_node->text, curr_node->next->text, MAX_WORD_LENGTH) == 0){ 
           after_duplicate = curr_node->next->next;
           free(curr_node->next); 
           curr_node->next = after_duplicate;   
       } 
       else
       { 
          curr_node = curr_node->next;  
       } 
    }
}

void remove_exceptions(node_t **in_toks_head, node_t **ex_toks_head){
    node_t *curr_node;
    node_t *ex_word;

    for(curr_node = *in_toks_head; curr_node != NULL; curr_node = curr_node->next){
        for(ex_word = *ex_toks_head; ex_word != NULL; ex_word = ex_word->next){
            if(strncmp(curr_node->text, ex_word->text, MAX_WORD_LENGTH) == 0){
               *in_toks_head =  delitem(*in_toks_head, ex_word->text);
            }
        }
    }
}

//code from lecture
node_t *delitem (node_t *listp, char *word){
    node_t *curr, *prev;
    prev = NULL;

    for (curr = listp; curr != NULL; curr = curr-> next) {
        if (strcmp(word, curr->text) == 0) {
            if (prev == NULL) {
                listp = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return listp;
        }
        prev = curr;
    }

    return listp;
}

void output(File *input, node_t **keywords){
    node_t *curr_node;
    int longest_key = 0;
    int word_occurences = 0;
    char line_buffer [MAX_LINE_LENGTH];
    char upper[MAX_WORD_LENGTH];

    //find and store the length of the longest word to help with indentation of output
	for(curr_node = *keywords; curr_node != NULL; curr_node = curr_node->next){
		int current_longest = strlen(curr_node->text);
		if(longest_key < current_longest){
			longest_key = current_longest;
		}
	}
    longest_key = (longest_key+2) * -1;

    curr_node = *keywords;
    //for each keyword
    for(curr_node = *keywords; curr_node != NULL; curr_node = curr_node->next){

    	//uppercase the word
        strncpy(upper, curr_node->text, MAX_WORD_LENGTH);
        int size = strlen(upper);
    	for(int m=0; m<size; m++){
    		upper[m] = toupper(upper[m]);
    	}

        //for each line
        for(int j=0; j<input->line_num; j++){
            strncpy(line_buffer, input->line[j].line, MAX_LINE_LENGTH);

            int line_length = strlen(line_buffer);
    		if(strcmp(&input->line[j].line[line_length-1], "\n") == 0){
    			input->line[j].line[line_length-1] = (char) 0;
    		}

            word_in_line(curr_node->text, line_buffer, &word_occurences);

            if(word_occurences == 1){
    			printf("%*s%s (%d)\n", longest_key, upper, input->line[j].line, j+1);
    		} else if (word_occurences > 1){
    			printf("%*s%s (%d*)\n", longest_key, upper, input->line[j].line, j+1);
    		}
    		
    		word_occurences = 0;
        }
    }
}

void word_in_line(char *keyword, char line[MAX_LINE_LENGTH], int *word_occurences){
    char *d;
    char token_temp[MAX_WORD_LENGTH];

    /*tokenizing file line and comparing key word to each word in the line
    and keeping track of how many times it appears*/
    d = strtok(line, " \n");
    while(d != NULL){
        strncpy(token_temp, d, MAX_WORD_LENGTH);

        for(int i=0 ; i<strlen(token_temp); i++){
            token_temp[i] = tolower(token_temp[i]);
        }

        if(strncmp(keyword, token_temp, MAX_WORD_LENGTH) == 0){
            *word_occurences = *word_occurences+1;
        }

        d = strtok(NULL, " \n");
    }
}

void free_list(node_t **words){
    node_t *temp;

    for(temp = *words; temp != NULL;){
        node_t *curr;
        curr = temp;
        temp = temp->next;
        free(curr);
    }
}