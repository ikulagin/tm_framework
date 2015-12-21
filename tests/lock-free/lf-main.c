#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include "thread_pool.h"
#include "lf-hashtable.h"

#define timer_t struct timeval
#define timer_read(t) gettimeofday( &(t), NULL);
#define timer_diff_usec(start, stop) \
    (((double)(stop.tv_sec * 1000000.0)  + (double)(stop.tv_usec)) - \
     ((double)(start.tv_sec * 1000000.0) + (double)(start.tv_usec ))) 

#define timer_diff_sec(start, stop) \
    (((double)(stop.tv_sec)  + (double)(stop.tv_usec / 1000000.0)) - \
     ((double)(start.tv_sec) + (double)(start.tv_usec / 1000000.0))) 

enum input_params {
    N_THREADS = 0,
    N_BUCKETS = 1,
    N_WORDS = 2,
    WORD_SIZE = 3,
    N_PARAMS,
};

int global_params[N_PARAMS];

lf_hashtable_t *global_ht = NULL;

void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 1) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

void hello_from_thread(void *arg)
{
    char **array_words = NULL;
    int n_words_per_thread = global_params[N_WORDS]/global_params[N_THREADS];

    array_words = (char **) calloc(sizeof(char *), n_words_per_thread);
    if (array_words == NULL)
        goto malloc_error;
    for (int i = 0; i < n_words_per_thread; i++) {
        array_words[i] = (char *) calloc(sizeof(char), global_params[WORD_SIZE]);
        if (array_words[i] == NULL)
            goto malloc_error;
    }

    for (int i = 0; i < n_words_per_thread; i++) {
        rand_str(array_words[i], global_params[WORD_SIZE]);
        lf_hashtable_insert(global_ht, array_words[i], array_words[i],
                            sizeof(char) * global_params[WORD_SIZE]);
    }


    for (int i = 0; i < n_words_per_thread; i++)
        free(array_words[i]);
    free(array_words);

    return ;
 malloc_error:
    fprintf(stderr, "malloc error\n");
    if (array_words)
        for (int i = 0; i < n_words_per_thread; i++)
            free(array_words[i]);
    free(array_words);
    exit(EXIT_FAILURE);
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

void parse_arg(int argc, char **argv)
{
    int opt = 0;

    global_params[N_THREADS] = 1;
    global_params[N_BUCKETS] = 10;
    global_params[N_WORDS] = 10;
    global_params[WORD_SIZE] = 10;

    /*
     * w - number of words
     * s - word size
     * b - number of buckets
     * t - number of threads
     * h - print help
     */
    opterr = 0;
    while((opt = getopt(argc, argv, "b:t:w:s:h")) != -1) {
        switch(opt) {
        case 'b':
            global_params[N_BUCKETS] = atoi(optarg);
            break;
        case 't':
            global_params[N_THREADS] = atoi(optarg);
            break;
        case 'w':
            global_params[N_WORDS] = atoi(optarg);
            break;
        case 's':
            global_params[WORD_SIZE] = atoi(optarg);
            break;
        case 'h':
            printf("./1_tm.out -b <BUCKETS> -w <WORDS> -s <WORD_SIZE> -t <THREADS>\n");
            printf("Print this help: ./1_tm.out -h \n");
            exit(EXIT_SUCCESS);
            break;
        case '?':
        default:
            fprintf(stderr, "unknown parameter: %c\n", opt);
            opterr++;
            break;
        }
    }

    if (opterr != 0) {
        fprintf(stderr, "Uncorrected parameters\n");
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char **argv)
{
    timer_t start, stop;
    thread_pool_t *pool;
    
    parse_arg(argc, argv);

    pool = thread_pool_init(global_params[N_THREADS]);
    global_ht = lf_hashtable_create(global_params[N_BUCKETS], hash);

    pool_startup(pool);
    
    timer_read(start);
    run_task(pool, hello_from_thread);
    timer_read(stop);

    pool_shutdown_thread(pool);
    //    tm_hashtable_print(global_ht);
    long total_size = lf_hashtable_total_size(global_ht);
    printf("The status of test is %s\n",
           (total_size == global_params[N_WORDS]) ?
           "true" : "false");
    printf("The hashtable size is %ld\n", total_size);
    printf("Total time: %f\n", timer_diff_sec(start, stop));

    lf_hashtable_delete(global_ht);
    thread_pool_finalize(pool);

    return 0;
}
