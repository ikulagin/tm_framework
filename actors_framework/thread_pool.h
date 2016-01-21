#ifndef THREAD_POOL_H
#define THREAD_POOL_H

typedef struct thread_pool_s thread_pool_t;
typedef struct worker_list_s worker_list_t;
typedef struct worker_des_s worker_des_t;

thread_pool_t *thread_pool_init();
void thread_pool_finalize(thread_pool_t *pool);

#endif // THREAD_POOL_H
