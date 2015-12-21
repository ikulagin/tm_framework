#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

#include "lf-list.h"

struct lf_list_s {
    lf_list_node_t *head;
};

struct lf_list_node_s {
    int state;
    void *dataPtr;
    lf_list_node_t *next;
};

/* The functions lock-free list node's */

lf_list_node_t *lf_list_node_create(int s, void *data, size_t data_size)
{
    lf_list_node_t *tmp = NULL;

    if ((tmp = calloc(sizeof(lf_list_node_t), 1)) == NULL) {
        fprintf(stderr, "[%s:%d] calloc error\n", __func__, __LINE__);
        return NULL;
    }
    
    if ((tmp->dataPtr = malloc(data_size)) == NULL) {
        free(tmp);
        return NULL;
    }
    memcpy(tmp->dataPtr, data, data_size);
    tmp->state = s;

    return tmp;
}

int lf_list_node_cas_state(lf_list_node_t *node, int state_orig, int state_new)
{
    return atomic_compare_exchange_weak(&node->state, &state_orig, state_new);
}

int lf_list_node_cas_next(lf_list_node_t *node, lf_list_node_t *next_orig,
                          lf_list_node_t *next_new)
{
    return atomic_compare_exchange_weak(&node->next, &next_orig, next_new);
}

void lf_list_node_delete(lf_list_node_t *node)
{
    free(node->dataPtr);
    free(node);
}

/* The functions lock-free list's */

lf_list_t *lf_list_create()
{
    lf_list_t *tmp = NULL;

    if ((tmp = calloc(sizeof(lf_list_t), 1)) == NULL) {
        fprintf(stderr, "[%s:%d] calloc error\n", __func__, __LINE__);
        return NULL;
    }

    return tmp;
}

int lf_list_insert(lf_list_t *list, lf_list_node_t *node)
{
    lf_list_node_t *h = NULL;

    while(1) {
        h = list->head;
        node->next = h;
        if (atomic_compare_exchange_weak(&list->head, &h, node))
            return 1;
    }

    return 0;
}

long lf_list_total_size(lf_list_t *list)
{
    long total_size = 0;

    for (lf_list_node_t *tmp = list->head; tmp != NULL; tmp = tmp->next)
        total_size++;

    return total_size;
}

void lf_list_delete(lf_list_t *list)
{
    for (lf_list_node_t *tmp = list->head; tmp != NULL; tmp = tmp->next)
        lf_list_node_delete(tmp);
    list->head = NULL;
    free(list);
}
