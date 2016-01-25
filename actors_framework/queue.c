#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct vector_s {
    int size;
    int tail;
    void **buf;
};

struct queue_cycl_s {
    int front;
    int tail;
    int max_size;
    void **cyclic_buf;
};

vector_t *vector_init(int size)
{
    vector_t *tmp = NULL;
    
    tmp = (vector_t *)calloc(1, sizeof(vector_t));
    if (tmp == NULL)
        goto vector_init_err;

    tmp->buf = (void **)calloc(size, sizeof(void *));
    if (tmp->buf == NULL)
        goto vector_init_err;

    tmp->size = size;
    tmp->tail = 0;

    return tmp;

 vector_init_err:
    fprintf(stderr, "vector_init error\n");
    if (tmp)
        free(tmp->buf);
    free(tmp);
    exit(EXIT_FAILURE);
}

int vector_push_back(vector_t *v, void *data)
{
    if (v->tail == v->size)
        return 0;

    v->buf[v->tail++] = data;

    return 1;
}

void *vector_pop_back(vector_t *v)
{
    return (v->tail == 0) ? NULL: v->buf[(v->tail--) - 1];
}

void vector_delete(vector_t *v)
{
    free(v->buf);
    free(v);
}

queue_cycl_t *queue_init()
{
    queue_cycl_t *tmp = NULL;

    tmp = (queue_cycl_t *)calloc(1, sizeof(queue_cycl_t));
    if (tmp == NULL)
        goto queue_init_err;

    tmp->max_size = QUEUE_BUF_SIZE + 1;

    tmp->cyclic_buf = (void **)calloc(QUEUE_BUF_SIZE + 1, sizeof(void *));
    if (tmp->cyclic_buf == NULL)
        goto queue_init_err;

    return tmp;

 queue_init_err:
    fprintf(stderr, "queue_init error\n");
    if (tmp)
        free(tmp->cyclic_buf);
    free(tmp);

    exit(EXIT_FAILURE);
}

int queue_size(queue_cycl_t *q)
{
    return (q->tail >= q->front) ? (q->tail - q->front) : (q->max_size - q->front + q->tail);
}

int queue_is_full(queue_cycl_t *q)
{
    return ((q->tail + 1) % q->max_size == q->front) ? 1 : 0;
}

int queue_is_empty(queue_cycl_t *q)
{
    return (q->front == q->tail) ? 1 : 0;
}

int queue_enqueue(queue_cycl_t *q, void *data, int data_size)
{
    if (queue_is_full(q))
        return 0;
    
    q->cyclic_buf[q->tail] = data;
    q->tail = (q->tail + 1) % q->max_size;

    return 1;
}

void *queue_dequeue(queue_cycl_t *q)
{
    void *tmp = NULL;
    if (queue_is_empty(q))
        return NULL;
    tmp = q->cyclic_buf[q->front];
    q->cyclic_buf[q->front] = NULL;
    q->front = (q->front + 1) % q->max_size;

    return tmp;
}

vector_t *queue_dequeueall(queue_cycl_t *q)
{
    int s = queue_size(q);
    vector_t *tmp = vector_init(s);
    void *data = NULL;

    while ((data = queue_dequeue(q)) != NULL) {
        if (!vector_push_back(tmp, data)) {
            queue_enqueue(q, data, 0); /* TODO?: In the case of if vector
                                          has been overflowing then
                                          we need to enqueue the data for avoiding
                                          memory leaks or exit(EXIT_FAILURE)?*/
            goto queue_dequeueall_err;
        }
    }

    return tmp;

 queue_dequeueall_err:
    fprintf(stderr, "[%s:%d] queue_dequeueall error\n", __func__, __LINE__);
    exit(EXIT_FAILURE);
}

void queue_delete(queue_cycl_t *q)
{
    free(q->cyclic_buf);
    free(q);
}
