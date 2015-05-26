#include <stdio.h>
#include <unistd.h>

#include "thread_pool.h"

void hello_from_thread(void *arg)
{
    printf("Hello from thread\n");
    get_thread_id();
}

int main()
{
    thread_pool_t *pool = thread_pool_init(2);

    pool_startup(pool);

    run_task(pool, hello_from_thread);

    
    return 0;
}
