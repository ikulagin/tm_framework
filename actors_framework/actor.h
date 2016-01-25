#ifndef ACTOR_BASE_H
#define ACTOR_BASE_H

typedef struct actor_msg_s actor_msg_t;
typedef struct actor_s actor_t;
typedef void (*action)(actor_msg_t *);

struct actor_msg_s {
    int type;
    void *data;
};

actor_t *actor_init(void **p, action h);

#endif // ACTOR_BASE_H

