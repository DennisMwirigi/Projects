void print_word(node_t *node, void *arg)
{
    char *format = (char *)arg;
    printf(format, node->text);
}

/*
 * Just showing the use of the linked-list routines.
 */
#ifdef DEBUG

void _demo() {
    printf("DEBUG: in _demo\n");
    char *words_german[] = {"Der", "Kater", "mit", "Hut."};
    int   words_german_len = 4;

    char *words_english[] = {"The", "cat", "in", "the", "hat."};
    int   words_english_len = 5;

    node_t *temp_node = NULL;
    node_t *head = NULL;

    int i;

    /* Add the words in German, at the front. */
    for (i = 0; i < words_german_len; i++) {
        temp_node = new_node(words_german[i]);
        head = add_front(head, temp_node);
    }

    /* Add the words in English, at the end. */
    for (i = 0; i < words_english_len; i++) {
        temp_node = new_node(words_english[i]);
        head = add_end(head, temp_node);
    }

    /* Print the list of words. */

    apply(head, print_word, "--> %s\n");

    /* Free up the memory. This is done rather deliberately
     * and manually.  Asserts are liberally used here as they
     * express state that *must* be true if all of the code is
     * correctly working.
     */

    temp_node = head;
    while (temp_node != NULL) {
        assert(temp_node != NULL);
        head = remove_front(head);
        free(temp_node);
        temp_node = head;
    }
   
    assert(head == NULL); 
}
#endif

/* 
 * Showing some simple usage of the linked-list routines.
 */

#ifdef DEBUG
    _demo();
#endif

    exit(0);

    //code from listy.c
void print_word(node_t *node, void *arg){
    char *format = (char *)arg;
    printf(format, node->text);
}

valgrind --leak-check=full --show-leak-kinds=all ./kwoc3 in01.txt