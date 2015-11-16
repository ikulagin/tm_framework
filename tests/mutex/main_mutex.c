#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "thread_pool.h"
#include "hashtable.h"

#define timer_t struct timeval
#define timer_read(t) gettimeofday( &(t), NULL);
#define timer_diff_usec(start, stop) \
    (((double)(stop.tv_sec * 1000000.0)  + (double)(stop.tv_usec)) - \
     ((double)(start.tv_sec * 1000000.0) + (double)(start.tv_usec ))) 

enum {
    NUM_WORDS = 10000,
    LENGTH_WORDS = 1000,
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
    //    long id = get_thread_id();
    printf("%s:%d\n", __func__, __LINE__);
    char array_words[NUM_WORDS][LENGTH_WORDS];
    printf("%s:%d\n", __func__, __LINE__);
    for (int i = 0; i < NUM_WORDS; i++) {
        rand_str(array_words[i], LENGTH_WORDS);
        printf("i = %d\n", i);
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
    timer_t start, stop;
    thread_pool_t *pool = thread_pool_init(atoi(argv[1]));
    global_ht = tm_hashtable_alloc(10, hash);

    pool_startup(pool);
    
    timer_read(start);
    run_task(pool, hello_from_thread);
    timer_read(stop);

    pool_shutdown_thread(pool);
    //    tm_hashtable_print(global_ht);
    long total_size = tm_hashtable_total_size(global_ht);
    printf("The status of test is %s\n",
           (total_size == atoi(argv[1]) * NUM_WORDS) ? "true" : "false");
    printf("The hashtable size is %ld\n", total_size);
    printf("Total time: %f\n", timer_diff_usec(start, stop));

    thread_pool_finalize(pool);
    return 0;
}
