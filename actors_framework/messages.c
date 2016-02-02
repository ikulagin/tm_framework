#include <stdio.h>
#include <stdlib.h>

#include "messages.h"

inline static actor_msg_t *new_msg();

inline static actor_msg_t *new_msg()
{
    actor_msg_t *m = NULL;

    m = (actor_msg_t *)calloc(1, sizeof(actor_msg_t));
    if (m == NULL)
        goto send_new_msg_err;

    return m;
    
 send_new_msg_err:
    fprintf(stderr, "[%s:%d] error during the creation msg\n", __func__, __LINE__);
    exit(EXIT_FAILURE);
}

void send_produce_or_consume(actor_t *to, int type)
{
    actor_msg_t *m = new_msg();
    
    m->type = type;
    m->m_dtor = NULL;
    m->data = NULL;

    actor_send_msg(to, m);
}

void send_exit(actor_t *to)
{
    actor_msg_t *m = new_msg();
    
    m->type = MSG_EXIT;
    m->m_dtor = NULL;
    m->data = NULL;

    actor_send_msg(to, m);
}

void msg_destroy(void *msg)
{
    actor_msg_t *m = (actor_msg_t *)msg;

    if (m->m_dtor && m->data)
        m->m_dtor(m->data);

    free(m);
}
