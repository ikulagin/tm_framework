#include <stdio.h>
#include <stdlib.h>

#include "actor.h"
#include "messages.h"

#include "custom_actors.h"

typedef struct consumer_data consumer_data_t;
struct consumer_data
{
    int *p_global;
};

static void consumer_dtor();
static void consumer(actor_t *iam, void *p, void *m);

actor_t *create_consumer(int *global)
{
    actor_t *a = NULL;
    consumer_data_t *d = NULL;

    d = (consumer_data_t *)calloc(1, sizeof(consumer_data_t));
    if (d == NULL)
        goto create_consumer_err;
    d->p_global = global;

    a = actor_spawn((void **)&d, consumer, consumer_dtor, msg_destroy);
    
    return a;
    
 create_consumer_err:
    fprintf(stderr, "[%s:%d] Creation consumer was failed", __func__, __LINE__);
    free(d);
    free(a);
    exit(EXIT_FAILURE);
}

static void consumer(actor_t *iam, void *p, void *m)
{
    actor_msg_t *msg = m;
    consumer_data_t *d = p;
    switch(msg->type) {
    case MSG_CONSUME:
        (*d->p_global)++;
        break;
    case MSG_EXIT:
        actor_mark_as_finished(iam);
    }
}

static void consumer_dtor(void *d)
{
    free(d);
}
