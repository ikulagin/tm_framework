#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread_pool.h"

void hello_from_thread(void *arg)
{
    printf("Hello from thread %ld\n", get_thread_id());
}

int main(int argc, char **argv)
{
    thread_pool_t *pool = thread_pool_init(atoi(argv[1]));

    pool_startup(pool);

    run_task(pool, hello_from_thread);

    pool_shutdown_thread(pool);

    thread_pool_finalize(pool);
    return 0;
}
