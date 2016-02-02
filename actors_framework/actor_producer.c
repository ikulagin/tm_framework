#include <stdio.h>
#include <stdlib.h>

#include "actor.h"
#include "messages.h"

#include "custom_actors.h"

typedef struct producer_data producer_data_t;
struct producer_data
{
    actor_t *consumer;
    int cnt;
};

static void producer_dtor();
static void producer(actor_t *iam, void *p, void *m);

actor_t *create_producer(actor_t *c, int n)
{
    actor_t *a = NULL;
    producer_data_t *d = NULL;

    d = (producer_data_t *)calloc(1, sizeof(producer_data_t));
    if (d == NULL)
        goto create_producer_err;
    d->cnt = n;
    d->consumer = c;

    a = actor_spawn((void **)&d, producer, producer_dtor, msg_destroy);
    
    return a;
    
 create_producer_err:
    fprintf(stderr, "[%s:%d] Creation producer was failed", __func__, __LINE__);
    free(d);
    free(a);
    exit(EXIT_FAILURE);
}

static void producer(actor_t *iam, void *param, void *msg)
{
    producer_data_t *p = (producer_data_t *)param;
    actor_msg_t *m = (actor_msg_t *)msg;

    switch(m->type) {
        case MSG_PRODUCE:
        while(p->cnt) {
            send_produce_or_consume(p->consumer, MSG_CONSUME);
            p->cnt--;
        }
        actor_mark_as_finished(iam);
        break;
    }
}

static void producer_dtor(void *d)
{
    free(d);
}
