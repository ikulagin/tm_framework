#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "thread_pool.h"

struct thread_pool_s {
    long pool_size;
    pthread_t *pool_threads;
    pthread_key_t id_tls;
    pthread_barrier_t pool_barrier;
    task_ptr task;
    long *id_threads;
};

static void *worker(void *);
static thread_pool_t *global_pool = NULL;

thread_pool_t *thread_pool_init(int num_threads)
{
    thread_pool_t *tmp = NULL;

    if ((tmp = (thread_pool_t *) malloc(sizeof(thread_pool_t))) == NULL) {
        exit(EXIT_FAILURE);
    }
    tmp->pool_size = num_threads;
    tmp->pool_threads = NULL;
    tmp->task = NULL;
    tmp->id_threads = NULL;
    
    if ((tmp->pool_threads = 
         (pthread_t *) malloc(sizeof(pthread_t) * num_threads)) == NULL) {
        exit(EXIT_FAILURE);
    }

    if (pthread_key_create(&tmp->id_tls, NULL) != 0) {
        exit(EXIT_FAILURE);
    }

    if (pthread_barrier_init(&tmp->pool_barrier, 0, num_threads) != 0) {
        exit(EXIT_FAILURE);
    }
    
    if ((tmp->id_threads = 
         (long *) malloc(sizeof(long) * num_threads)) == NULL) {
        exit(EXIT_FAILURE);
    }
    for (long i = 0; i < num_threads; i++) {
        tmp->id_threads[i] = i;
    }

    global_pool = tmp;
    
    return tmp;
}

void pool_startup(thread_pool_t *pool)
{
    if (pool == NULL) {
        exit(EXIT_FAILURE);
    }

    for (long i = 1; i <  pool->pool_size; i++) {
        pthread_create(&pool->pool_threads[i], 0, worker, &pool->id_threads[i]);
    }
}

void run_task(thread_pool_t *pool, task_ptr task)
{
    pool->task = task;
    worker(&pool->id_threads[0]);
}

long get_thread_id()
{
    printf("%p\n", pthread_getspecific(global_pool->id_tls));
    return 0;
}

static void *worker(void *arg)
{
    long thread_id = *(long*)arg;

    printf("setspecific %p\n", arg);
    pthread_setspecific(global_pool->id_tls, arg);

    while(1) {
        printf("thread %ld is ready\n", thread_id);
        pthread_barrier_wait(&global_pool->pool_barrier);
        global_pool->task(NULL);
        printf("thread %ld is complete\n", thread_id);
        pthread_barrier_wait(&global_pool->pool_barrier);
        if (thread_id == 0)
            break;
    }
    
    return NULL;
}
