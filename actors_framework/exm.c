#include <stdio.h>
#include <stdlib.h>

#include "actor.h"
#include "messages.h"
#include "custom_actors.h"

enum {
    PRODUCE_COUNT = 1024,
};

int global = 0;

int main()
{
    actor_t *consumer = create_consumer(&global);
    actor_t *producer1 = create_producer(consumer, PRODUCE_COUNT);
    actor_t *producer2 = create_producer(consumer, PRODUCE_COUNT);

    send_produce_or_consume(producer1, MSG_PRODUCE);
    send_produce_or_consume(producer2, MSG_PRODUCE);
    actor_join(producer1);
    actor_join(producer2);
    send_exit(consumer);
    actor_join(consumer);
    printf("global = %d\n", global);

    return 0;
}
