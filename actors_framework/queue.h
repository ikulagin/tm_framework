#ifndef QUEUE_H
#define QUEUE_H

typedef struct vector_s vector_t;
typedef struct queue_cycl_s queue_cycl_t;

vector_t *vector_init(int size);
int vector_push_back(vector_t *v, void *data);
void *vector_pop_back(vector_t *v);
void vector_delete(vector_t *v);

queue_cycl_t *queue_init();
int queue_size(queue_cycl_t *q);
int queue_is_full(queue_cycl_t *q);
int queue_is_empty(queue_cycl_t *q);
int queue_enqueue(queue_cycl_t *q, void *data, int data_size);
void *queue_dequeue(queue_cycl_t *q);
vector_t *queue_dequeueall(queue_cycl_t *q);
void queue_delete(queue_cycl_t *q);

#endif // QUEUE_H
