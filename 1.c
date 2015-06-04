#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread_pool.h"
#include "hashtable.h"

enum {
    NUM_WORDS = 1024,
    LENGTH_WORDS = 10,
};

hashtable_t *global_ht = NULL;

void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

void hello_from_thread(void *arg)
{
    long id = get_thread_id();
    char array_words[NUM_WORDS][LENGTH_WORDS];

    printf("Hello from thread %ld\n", id);

    for (int i = 0; i < NUM_WORDS; i++) {
        rand_str(array_words[i], LENGTH_WORDS);
        printf("array_words[%d] = [%s]\n", i, array_words[i]);
        tm_hashtable_insert(global_ht, array_words[i], array_words[i]);
    }
}

unsigned long hash(const void *key)
{
    unsigned long hash_val = 0;
    char *str = (char *)key;
    long c;

    while((c = *str++) != '\0') {
        hash_val = c + (hash_val << 6) + (hash_val << 16) - hash_val;
    }

    return hash_val;
}

int main(int argc, char **argv)
{
    thread_pool_t *pool = thread_pool_init(atoi(argv[1]));
    global_ht = tm_hashtable_alloc(100, hash);

    pool_startup(pool);

    run_task(pool, hello_from_thread);

    pool_shutdown_thread(pool);
    tm_hashtable_print(global_ht);

    thread_pool_finalize(pool);
    return 0;
}
