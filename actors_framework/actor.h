#ifndef ACTOR_BASE_H
#define ACTOR_BASE_H

typedef struct actor_msg_s actor_msg_t;
typedef struct actor_s actor_t;
typedef void (*action)(actor_t *, void *, actor_msg_t *);

struct actor_msg_s {
    int type;
    void *data;
};

actor_t *actor_spawn(void **p, action h);
void actor_send_msg(actor_t *a, actor_msg_t *msg);
void actor_mark_as_finished(actor_t *a);
void actor_join(actor_t *a);
actor_msg_t *actor_msg_create(int type);

#endif // ACTOR_BASE_H

