#ifndef ACTOR_BASE_H
#define ACTOR_BASE_H

typedef struct actor_s actor_t;
typedef void (*action)(actor_t *, void *, void *);
typedef void (*msg_destroyer)(void *);
typedef void (*actor_data_dtor)(void *);

actor_t *actor_spawn(void **p,
                     action h,
                     actor_data_dtor d_dtor,
                     msg_destroyer m_destroyer);
void actor_send_msg(actor_t *a, void *msg);
void actor_mark_as_finished(actor_t *a);
void actor_join(actor_t *a);

#endif // ACTOR_BASE_H

