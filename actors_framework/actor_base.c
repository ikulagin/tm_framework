#include <stdlib.h>

#include "actor_base.h"

enum {
    ACTORS_NUM = 10,
};

struct actor_base_s {
    int id;
    runner run_actor;
};

typedef struct actor_array_s {
    int size;
    actor_base_t **actors;
} actor_array_t;

static actor_array_t *actor_array = NULL;

int actor_framework_init()
{
    actor_array = (actor_array_t *)calloc(1, sizeof(actor_array_t));
    if (actor_array == NULL)
        return -1;

    actor_array->actors = (actor_base_t **) calloc(ACTORS_NUM,
                                                   sizeof(actor_base_t *));
    if (actor_array->actors)

    return 0;
}

int *actor_create()
{
    actor_base_t *tmp = NULL;

    tmp = (actor_base_t *)calloc(1, sizeof(actor_base_t));
    if (!tmp)
        goto actor_create_err;

    return tmp;

 actor_create_err:
    free(tmp);
    return NULL;
}
