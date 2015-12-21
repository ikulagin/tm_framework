#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

#include "lf-list.h"
#include "lf-hashtable.h"

struct lf_hashtable_s {
    int size;
    hash_fnc hash;
    lf_list_t **buckets;
};

lf_hashtable_t *lf_hashtable_create(int n_buckets, hash_fnc hash_func)
{
    lf_hashtable_t *tmp = NULL;

    if ((tmp = calloc(sizeof(lf_hashtable_t), 1)) == NULL) {
        fprintf(stderr, "[%s:%d] malloc error\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    tmp->hash = hash_func;
    tmp->size = n_buckets;
    if ((tmp->buckets = (lf_list_t **)calloc(sizeof(lf_list_t *), n_buckets)) == NULL) {
        fprintf(stderr, "[%s:%d] malloc error\n", __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_buckets; i++) {
        if ((tmp->buckets[i] = lf_list_create()) == NULL) {
            fprintf(stderr, "[%s:%d] malloc error\n", __func__, __LINE__);
            exit(EXIT_FAILURE);
        }
    }

    return tmp;
}

int lf_hashtable_insert(lf_hashtable_t *ht, void *key, void *data_ptr, size_t data_size)
{
    unsigned long buck_id = ht->hash(key) % ht->size;
    lf_list_node_t *tmp = lf_list_node_create(INSERT, data_ptr, data_size);

    if (tmp == NULL) {
       fprintf(stderr, "[%s:%d] malloc error\n", __func__, __LINE__);
       exit(EXIT_FAILURE);
    }

    return lf_list_insert(ht->buckets[buck_id], tmp);
}

long lf_hashtable_total_size(lf_hashtable_t *ht)
{
    long total_size = 0;

    for (int i = 0; i < ht->size; i++)
        total_size += lf_list_total_size(ht->buckets[i]);
    
    return total_size;
}

void lf_hashtable_delete(lf_hashtable_t *ht)
{
    for (int i = 0; i < ht->size; i++) {
        lf_list_delete(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
}
