
typedef void (*task_ptr)(void*);
typedef struct thread_pool_s thread_pool_t;

extern thread_pool_t *thread_pool_init(int num_threads);
extern void thread_pool_finalize(thread_pool_t *pool);
extern void pool_startup(thread_pool_t *pool);
extern void pool_shutdown_thread(thread_pool_t *pool);
extern void run_task(thread_pool_t *pool, task_ptr task);
extern long get_thread_id();
