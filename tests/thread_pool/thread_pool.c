#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>

#include "thread_pool.h"

struct thread_pool_s {
    long pool_size;
    pthread_t *pool_threads;
    pthread_key_t id_tls;
    pthread_barrier_t pool_barrier;
    atomic_bool do_shutdown;
    task_ptr task;
    long *id_threads;
};

static void *worker(void *);
static thread_pool_t *global_pool = NULL;

thread_pool_t *thread_pool_init(int num_threads)
{
    thread_pool_t *tmp = NULL;

    if (num_threads < 0)
        exit(EXIT_FAILURE);
    
    if ((tmp = (thread_pool_t *) malloc(sizeof(thread_pool_t))) == NULL) {
        exit(EXIT_FAILURE);
    }
    atomic_store(&tmp->do_shutdown, 0);
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

void thread_pool_finalize(thread_pool_t *pool)
{
    if (pool == NULL) {
        exit(EXIT_FAILURE);
    }

    free(pool->pool_threads); pool->pool_threads = NULL;
    free(pool->id_threads); pool->id_threads = NULL;
    pthread_key_delete(pool->id_tls);
    pthread_barrier_destroy(&pool->pool_barrier);
    free(pool);
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

void pool_shutdown_thread(thread_pool_t *pool)
{
    atomic_store_explicit(&pool->do_shutdown, 1, memory_order_release);
    pthread_barrier_wait(&global_pool->pool_barrier);

    for (long i = 1; i <  pool->pool_size; i++) {
        pthread_join(pool->pool_threads[i], NULL);
        printf("master thread: thread %ld is completed\n", pool->id_threads[i]);
    } 
}

long get_thread_id()
{
    return *(long *)pthread_getspecific(global_pool->id_tls);
}

static void *worker(void *arg)
{
    long thread_id = *(long*)arg;

    pthread_setspecific(global_pool->id_tls, arg);

    while(1) {
        printf("thread %ld is ready\n", thread_id);
        pthread_barrier_wait(&global_pool->pool_barrier);

        if (atomic_load_explicit(&global_pool->do_shutdown, memory_order_acquire) == 1)
            break;

        if (global_pool->task != NULL) {
            global_pool->task(NULL);
        }
        
        printf("thread %ld has completed the task\n", thread_id);
        pthread_barrier_wait(&global_pool->pool_barrier);

        if (thread_id == 0)
            break;
    }
    printf("thread %ld is being finished\n", thread_id);
    
    return NULL;
}
