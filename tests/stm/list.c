#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"

typedef struct list_node_s {
    void *dataPtr;
    struct list_node_s *next;
} list_node_t;

struct list_s {
    list_node_t *head;
    long size;
};

// Declaration section
__attribute__((transaction_safe))
list_node_t *list_node_alloc(void *data, size_t data_size);
__attribute__((transaction_safe))
void list_node_free(list_node_t *node_ptr);
// End

list_t *list_alloc()
{
    list_t *tmp = NULL;

    if ((tmp = malloc(sizeof(list_t))) == NULL) {
        exit(EXIT_FAILURE);
    }

    tmp->head = NULL;
    tmp->size = 0;

    return tmp;
}

__attribute__((transaction_safe))
list_node_t *list_node_alloc(void *data, size_t data_size)
{
    list_node_t *tmp = NULL;

    if ((tmp = malloc(sizeof(list_node_t))) == NULL) {
        return NULL;
    }
    tmp->next = NULL;

    if ((tmp->dataPtr = malloc(data_size)) == NULL) {
        free(tmp);
        return NULL;
    }
    memcpy(tmp->dataPtr, data, data_size);
    
    return tmp;
}

__attribute__((transaction_safe))
bool list_insert(list_t *list_ptr, void *data, size_t data_size)
{
    list_node_t *current = list_node_alloc(data, data_size);
    list_node_t *next = NULL;

    __transaction_atomic {
        next = list_ptr->head;
        current->next = next;
        list_ptr->head = current;
        list_ptr->size++;
    }

    return true;
}

void list_print(list_t *list_ptr)
{
    printf("size = %ld [ ", list_ptr->size);
    for (list_node_t *ptr = list_ptr->head; ptr != NULL; ptr = ptr->next) {
        printf("%s, ", (char *)ptr->dataPtr);
    }
    printf("]\n");
}

long list_total_size(list_t *list_ptr)
{
    long total_size = 0;

    for (list_node_t *ptr = list_ptr->head; ptr != NULL; ptr = ptr->next)
        total_size++;

    if (total_size != list_ptr->size)
        printf("warn: the size is not same %ld != %ld\n", total_size, list_ptr->size);
    
    return total_size;
}

__attribute__((transaction_safe))
void list_node_free(list_node_t *node_ptr)
{
    if (node_ptr != NULL) {
        free(node_ptr->dataPtr);
        list_node_free(node_ptr->next);
        node_ptr->next = NULL;
        free(node_ptr);
    }
}

__attribute__((transaction_safe))
void list_free(list_t *list_ptr)
{
    list_node_t *node_ptr = list_ptr->head;

    list_node_free(node_ptr);
    list_ptr->head = NULL;
    list_ptr->size = 0;
    free(list_ptr);
}

