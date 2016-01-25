#include <stdlib.h>
#include <pthread.h>

#include "actor.h"
#include "queue.h"

struct actor_s {
    pthread_t thread;
    pthread_mutex_t m;
    pthread_cond_t cond;
    queue_cycl_t *q;
    void *params;
    action handler;
};

static void *actor_runner(void *arg);

actor_t *actor_init(void **p, action h)
{
    actor_t *a = (actor_t *)calloc(1, sizeof(actor_t));

    if(a == NULL)
        goto actor_init_err;

    pthread_mutex_init(&a->m, NULL);
    pthread_cond_init(&a->cond, NULL);
    a->q = queue_init();
    a->params = *p;
    a->handler = h;

    pthread_create(&a->thread, NULL, actor_runner, a);

    return a;
    
 actor_init_err:
    fprintf(stderr, "[%s:%d] erro initialization\n exit\n", __func__, __LINE__);
    exit(EXIT_FAILURE);
}

static void *actor_runner(void *arg)
{
    actor_t *iam = (actor_t *)arg;

    while (1) {
        pthread_mutex_lock(&iam->m);
        if (!queue_size(iam->q))
            pthread_cond_wait(&iam->cond, &iam->m);
        vector_t *v = queue_dequeueall(iam->q);
        pthread_mutex_unlock(&iam->m);

        int *data = NULL, exit = 0;
        while ((data = vector_pop_back(v)) != NULL) {
            //            iam->handler()
        }
        free(data);
        }
        vector_delete(v);
        if (exit)
            break;
    }

    for (int i = 0; i < 50; i++) {
        if (buf[i] != n_senders)
            fprintf(stderr, "ERROR!!!!!!!!\n");
    }

    return NULL;
}
