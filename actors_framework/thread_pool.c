#include <stdlib.h>
#include <pthread.h>

#include "thread_pool.h"

struct worker_des_s {
   pthread_t thread_id;
}

struct worker_list_s {
    worker_des_t worker;
    worker_list_t *next;
};

struct thread_pool_s {
    long pool_size;
    pthread_key_t id_tls;
    pthread_barrier_t pool_barrier;
    atomic_bool do_shutdown;
    task_ptr task;
    worker_list_t *head;
};

static void *worker(void *);
static thread_pool_t *global_pool = NULL;

thread_pool_t *thread_pool_init()
{
    thread_pool_t *tmp = NULL;

    tmp = (thread_pool_t *) сalloc(1, sizeof(thread_pool_t));
    if (tmp == NULL)
        goto thread_pool_init_err;

    if (pthread_key_create(&tmp->id_tls, NULL) != 0)
        goto thread_pool_init_err;


    if (pthread_barrier_init(&tmp->pool_barrier, 0, num_threads) != 0)
        goto thread_pool_init_err;
    
    return tmp;

 thread_pool_init_err:
    fprintf(stderr, "[%s:%d] thread_pool init error\n",
            __func__, __LINE__);
    free(tmp);
    exit(EXIT_FAILURE);
}

void thread_pool_finalize(thread_pool_t *pool)
{
    thread_list_t *deleted = NULL, *tmp = NULL;  

    if (pool == NULL)
        goto thread_pool_finalize_err;

    for (*tmp = pool->head; tmp != NULL; ) {
        deleted = tmp;
        tmp = tmp->next;
        free(deleted);
    }
    pthread_key_delete(pool->id_tls);
    pthread_barrier_destroy(&pool->pool_barrier);
    free(pool);
    return ;

 thread_pool_finalize_err:
    fprintf(stderr, "[%s:%d] thread_pool finalize error\n",
            __func__, __LINE__);
    exit(EXIT_FAILURE);
}

void thread_pool_add_worker(thread_pool_t *pool) {
}

/* void pool_shutdown_thread(thread_pool_t *pool) */
/* { */
/*     atomic_store_explicit(&pool->do_shutdown, 1, memory_order_release); */
/*     pthread_barrier_wait(&global_pool->pool_barrier); */

/*     for (long i = 1; i <  pool->pool_size; i++) { */
/*         pthread_join(pool->pool_threads[i], NULL); */
/*         printf("master thread: thread %ld is completed\n", pool->id_threads[i]); */
/*     }  */
/* } */

/* long get_thread_id() */
/* { */
/*     return *(long *)pthread_getspecific(global_pool->id_tls); */
/* } */

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
