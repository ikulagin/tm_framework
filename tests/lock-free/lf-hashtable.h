#ifndef LF_HASHTABLE_H
#define LF_HASHTABLE_H

typedef struct lf_hashtable_s lf_hashtable_t;

typedef unsigned long (*hash_fnc)(const void *);

lf_hashtable_t *lf_hashtable_create(int n_buckets, hash_fnc hash_func);
int lf_hashtable_insert(lf_hashtable_t *ht, void *key, void *data_ptr, size_t data_size);
long lf_hashtable_total_size(lf_hashtable_t *ht);
void lf_hashtable_delete(lf_hashtable_t *ht);

#endif
