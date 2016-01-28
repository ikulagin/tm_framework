#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "actor.h"
#include "queue.h"

struct actor_s {
    pthread_t thread;
    pthread_mutex_t m;
    pthread_cond_t cond;
    queue_cycl_t *q;
    action handler;
    char is_finish;
    actor_data_dtor d_dtor;
    void *params;
};

static void *actor_runner(void *arg);
static void actor_delete(actor_t *a);

actor_t *actor_spawn(void **p, action h, actor_data_dtor d_dtor)
{
    actor_t *a = (actor_t *)calloc(1, sizeof(actor_t));

    if(a == NULL)
        goto actor_init_err;

    pthread_mutex_init(&a->m, NULL);
    pthread_cond_init(&a->cond, NULL);
    a->q = queue_init();
    if (p != NULL) {
        a->params = *p;
        *p = NULL;
    }

    a->handler = h;
    a->is_finish = 0;
    a->d_dtor = d_dtor;

    pthread_create(&a->thread, NULL, actor_runner, a);

    return a;
    
 actor_init_err:
    fprintf(stderr, "[%s:%d] error initialization\n exit\n", __func__, __LINE__);
    exit(EXIT_FAILURE);
}


/*
 * TODO: In case of the overflowing queue the sender
 *       needs to wait the emptying queue.
 */
void actor_send_msg(actor_t *a, actor_msg_t *msg)
{
    pthread_mutex_lock(&a->m);
    queue_enqueue(a->q, msg, 0);
    pthread_cond_signal(&a->cond);
    pthread_mutex_unlock(&a->m);
}

actor_msg_t *actor_msg_create(int type, msg_dtor m_dtor)
{
    actor_msg_t *msg = NULL;

    msg = (actor_msg_t *)calloc(1, sizeof(actor_msg_t));
    if (msg == NULL)
        goto actor_msg_create_err;
    msg->type = type;
    msg->m_dtor = m_dtor;
    
    return msg;
 actor_msg_create_err:
    fprintf(stderr, "[%s:%d] error during the creation msg\n", __func__, __LINE__);
    exit(EXIT_FAILURE);
}

void actor_mark_as_finished(actor_t *a)
{
    a->is_finish = 1;
}

void actor_join(actor_t *a)
{
    pthread_join(a->thread, NULL);
    actor_delete(a);
}

static void *actor_runner(void *arg)
{
    actor_t *iam = (actor_t *)arg;

    while (!iam->is_finish) {
        pthread_mutex_lock(&iam->m);
        if (!queue_size(iam->q))
            pthread_cond_wait(&iam->cond, &iam->m);
        vector_t *v = queue_dequeueall(iam->q);
        pthread_mutex_unlock(&iam->m);

        actor_msg_t *msg = NULL;
        while ((msg = vector_pop_back(v)) != NULL) {
            iam->handler(iam, iam->params, msg);
            if (msg->m_dtor)
                msg->m_dtor(msg);
        }
        vector_delete(v);
    }

    return NULL;
}

static void actor_delete(actor_t *a)
{
    if (a->d_dtor && a->params)
        a->d_dtor(a->params);

    pthread_mutex_destroy(&a->m);
    pthread_cond_destroy(&a->cond);
    
    vector_t *v = queue_dequeueall(a->q);
    actor_msg_t *msg = NULL;
    while ((msg = vector_pop_back(v)) != NULL) {
        msg->m_dtor != NULL ? msg->m_dtor(msg) : free(msg);
    }

    vector_delete(v);
    queue_delete(a->q);

    free(a);
}
