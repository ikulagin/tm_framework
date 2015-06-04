#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hashtable.h"

struct hashtable_s {
    int *buckets;
    long num_buckets;
    long size;
    hash_fnc hash;
};

hashtable_t *tm_hashtable_alloc(long init_num_bucks, hash_fnc hash)
{
    hashtable_t *tmp = NULL;
    
    if ((tmp = malloc(sizeof(hashtable_t))) == NULL) {
        exit(EXIT_FAILURE);
    }

    if ((tmp->buckets = malloc(sizeof(int) * init_num_bucks)) == NULL) {
        exit(EXIT_FAILURE);
    }

    memset(tmp->buckets, 0, sizeof(int) * init_num_bucks);
    tmp->num_buckets = init_num_bucks;
    tmp->size = 0;
    tmp->hash = hash;

    return tmp;
}

void tm_hashtable_free(hashtable_t *ht_ptr)
{
    free(ht_ptr->buckets);
    ht_ptr->buckets = NULL;
    ht_ptr->size = 0;
    ht_ptr->num_buckets = 0;
    free(ht_ptr);
}

bool tm_hashtable_insert (hashtable_t *ht_ptr, void *key, void *data)
{
    unsigned long buck_id = ht_ptr->hash(key) % ht_ptr->num_buckets;

    ht_ptr->size++;
    ht_ptr->buckets[buck_id]++;

    return true;
}

void tm_hashtable_print(hashtable_t *ht_ptr)
{
    printf("hashtable [%p]->size = %ld\n", ht_ptr, ht_ptr->size);
    for (int i = 0; i < ht_ptr->num_buckets; i++) {
        printf("hashtable [%p]->buckets[%d] = %d\n", ht_ptr, i, ht_ptr->buckets[i]);
    }
}
