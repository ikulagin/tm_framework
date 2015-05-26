
typedef void (*task_ptr)(void*);
typedef struct thread_pool_s thread_pool_t;

thread_pool_t *thread_pool_init(int num_threads);
void pool_startup(thread_pool_t *pool);
void run_task(thread_pool_t *pool, task_ptr task);
long get_thread_id();
