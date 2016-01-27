#include <stdio.h>
#include <stdlib.h>

#include "actor.h"

enum msg_types {
    MSG_PRODUCE,
    MSG_INCREMENT,
    MSG_EXIT,
};

typedef struct actor_producer_data producer_data_t;
struct actor_producer_data
{
    actor_t *incrementor;
    int cnt;
};

actor_msg_t *msg_producer_begin();
actor_msg_t *msg_increment();
actor_msg_t *msg_actor_exit();

producer_data_t *producer_data_ctor(actor_t *a, int n)
{
    producer_data_t *d = NULL;
    d = (producer_data_t *)calloc(1, sizeof(producer_data_t));
    if (d == NULL)
        return NULL;
    d->cnt = n;
    d->incrementor = a;
    
    return d;
}
void producer_data_dtor(void *d)
{
    free(d);
}


int global = 0;
void actor_incrementer(actor_t *iam, void *param, actor_msg_t *msg)
{
    switch(msg->type) {
    case MSG_INCREMENT:
        global++;
        break;
    case MSG_EXIT:
        actor_mark_as_finished(iam);
    }
}

void actor_producer(actor_t *iam, void *param, actor_msg_t *msg)
{
    producer_data_t *data = param;
    switch(msg->type) {
    case MSG_PRODUCE:
        while(data->cnt) {
            actor_send_msg(data->incrementor, msg_increment());
            data->cnt--;
        }
        actor_mark_as_finished(iam);
        break;
    }
}

/*
 * Messages begin
 */
void msg_actor_exit_dtor(actor_msg_t *m);
void msg_increment_dtor(actor_msg_t *m);
void msg_producer_begin_dtor(actor_msg_t *m);

actor_msg_t *msg_producer_begin()
{
    actor_msg_t *msg = actor_msg_create(MSG_PRODUCE, msg_producer_begin_dtor);
    msg->data = NULL;

    return msg;
}

void msg_producer_begin_dtor(actor_msg_t *m)
{
    free(m->data);
    free(m);
}

actor_msg_t *msg_increment()
{
    actor_msg_t *msg = actor_msg_create(MSG_INCREMENT, msg_increment_dtor);
    msg->data = NULL;

    return msg;
}

void msg_increment_dtor(actor_msg_t *m)
{
    free(m->data);
    free(m);
}

actor_msg_t *msg_actor_exit()
{
    actor_msg_t *msg = actor_msg_create(MSG_EXIT, msg_actor_exit_dtor);
    msg->data = NULL;

    return msg;
}

void msg_actor_exit_dtor(actor_msg_t *m)
{
    free(m->data);
    free(m);
}

int main()
{
    actor_t *incrementer = actor_spawn(NULL, actor_incrementer, NULL);
    producer_data_t *p_data1 = producer_data_ctor(incrementer, 1024);
    producer_data_t *p_data2 = producer_data_ctor(incrementer, 1024);
    actor_t *producer1 = actor_spawn((void **)&p_data1,
                                     actor_producer,
                                     producer_data_dtor);
    actor_t *producer2 = actor_spawn((void **)&p_data2,
                                     actor_producer,
                                     producer_data_dtor);
    actor_send_msg(producer1, msg_producer_begin());
    actor_send_msg(producer2, msg_producer_begin());
    actor_join(producer1);
    actor_join(producer2);
    actor_send_msg(incrementer, msg_actor_exit());
    actor_join(incrementer);
    printf("global = %d\n", global);

    return 0;
}
