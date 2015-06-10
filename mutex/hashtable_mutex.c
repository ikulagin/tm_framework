#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hashtable_mutex.h"
#include "list_mutex.h"

struct hashtable_s {
    list_t **buckets;
    long num_buckets;
    long size;
    hash_fnc hash;
};

//Declaration section
size_t tm_strlen(const char *s);
//end

hashtable_t *tm_hashtable_alloc(long init_num_bucks, hash_fnc hash)
{
    hashtable_t *tmp = NULL;
    
    if ((tmp = malloc(sizeof(hashtable_t))) == NULL) {
        exit(EXIT_FAILURE);
    }

    if ((tmp->buckets = malloc(sizeof(list_t *) * init_num_bucks)) == NULL) {
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < init_num_bucks; i++) {
        if ((tmp->buckets[i] = list_alloc()) == NULL) {
            exit(EXIT_FAILURE);
        }
    }

    tmp->num_buckets = init_num_bucks;
    tmp->size = 0;
    tmp->hash = hash;

    return tmp;
}

void tm_hashtable_free(hashtable_t *ht_ptr)
{
    for (int i = 0; i < ht_ptr->num_buckets; i++) {
        list_free(ht_ptr->buckets[i]);
    }
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
    list_insert(ht_ptr->buckets[buck_id], data, tm_strlen(data));

    return true;
}

void tm_hashtable_print(hashtable_t *ht_ptr)
{
    printf("hashtable [%p]->size = %ld\n", ht_ptr, ht_ptr->size);
    for (int i = 0; i < ht_ptr->num_buckets; i++) {
        printf("hashtable [%p]->buckets[%d] = %p\n", ht_ptr, i, ht_ptr->buckets[i]);
        list_print(ht_ptr->buckets[i]);
    }
}

long tm_hashtable_total_size(hashtable_t *ht_ptr)
{
    long total_size = 0;

    for (int i = 0; i < ht_ptr->num_buckets; i++) {
        total_size += list_total_size(ht_ptr->buckets[i]);
    }

    return total_size;
}

size_t tm_strlen(const char *s)
{
    char *str = (char *)s;
    char c;
    size_t size = 0;

    while((c = *str++) != '\0')
        size++;

    return size;
}

