#ifndef MESSAGES_H
#define MESSAGES_H

#include "actor.h"

enum {
    MSG_PRODUCE,
    MSG_CONSUME,
    MSG_EXIT,
};

typedef struct actor_msg_s actor_msg_t;
typedef void (*msg_dtor)(actor_msg_t *);

struct actor_msg_s {
    int type;
    msg_dtor m_dtor;
    void *data;
};

void send_produce_or_consume(actor_t *to, int type); /*There may be arguments for sending*/
void send_exit(actor_t *to);
void msg_destroy(void *msg);

#endif // MESSAGES_H
