#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

enum {
    QUEUE_BUF_SIZE = 100000,
};

typedef struct vector_s {
    int size;
    int tail;
    void **buf;
} vector_t;

typedef struct queue_cycl_s {
    int front;
    int tail;
    int max_size;
    void **cyclic_buf;
} queue_cycl_t;

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

void queue_foreach(queue_cycl_t *q, void (*fnc)(void*))
{
    int tmp = q->front;
    while (tmp != q->tail) {
        fnc(q->cyclic_buf[tmp]);
        tmp = (tmp + 1) % q->max_size;
    }
}

void print_int(void *arg)
{
    printf("%d ", *(int *)arg);
}

typedef struct actor_s {
    pthread_t therad;
    pthread_mutex_t m;
    pthread_cond_t cond;
    queue_cycl_t *q;
} actor_t;

void actor_send_to(actor_t *a, void *msg);

void *sender(void *arg)
{
    actor_t *receiver = (actor_t *)arg;
    int *msg = NULL;

    for (int i = 0; i < 50; i++) {
        msg = (int *)calloc(1, sizeof(int));
        *msg = i;
        actor_send_to(receiver, msg);
    }

    return NULL;
}
int n_senders;
void *actor_runner(void *arg)
{
    actor_t *iam = (actor_t *)arg;
    int buf[50];
    memset(buf, 0, sizeof(int) * 50);
    while (1) {
        pthread_mutex_lock(&iam->m);
        if (!queue_size(iam->q))
            pthread_cond_wait(&iam->cond, &iam->m);
        vector_t *v = queue_dequeueall(iam->q);
        pthread_mutex_unlock(&iam->m);

        int *data = NULL, exit = 0;
        while ((data = vector_pop_back(v)) != NULL) {
            if (*data == -1) {
                exit = 1;
            } else {
                buf[*data]++;
            }
            free(data);
        }
        vector_delete(v);
        if (exit)
            break;
    }

    for (int i = 0; i < 50; i++) {
        if (buf[i] != n_senders)
            fprintf(stderr, "ERROR!!!!!!!!\n");
    }

    return NULL;
}


actor_t * actor_init()
{
    actor_t *tmp = NULL;

    tmp = (actor_t *)calloc(1, sizeof(actor_t));
    if (tmp == NULL)
        goto actor_init_err;

    pthread_mutex_init(&tmp->m, NULL);
    pthread_cond_init(&tmp->cond, NULL);
    tmp->q = queue_init();
    pthread_create(&tmp->therad, NULL, actor_runner, tmp);

    return tmp;
 actor_init_err:
    fprintf(stderr, "[%s:%d] actor_init error\n", __func__, __LINE__);    
    exit(EXIT_FAILURE);
}

void actor_send_to(actor_t *a, void *msg)
{
    pthread_mutex_lock(&a->m);
    queue_enqueue(a->q, msg, 0);
    pthread_cond_signal(&a->cond);
    pthread_mutex_unlock(&a->m);
}

void actor_finalize(actor_t *a)
{
    pthread_join(a->therad, NULL);
    queue_delete(a->q);
    pthread_mutex_destroy(&a->m);
    pthread_cond_destroy(&a->cond);
    free(a);
}

int main(int argc, char **argv)
{
    actor_t *actor = NULL;
    pthread_t *senders_id = NULL;

    if (argc != 2) {
        fprintf(stderr,
                "[%s:%d] Start failed. A number of senders is not specified\n",
                __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    n_senders = atoi(argv[1]);

    senders_id = (pthread_t *)calloc(n_senders, sizeof(pthread_t));

    actor = actor_init();

    for (int i = 0; i < n_senders; i++) {
        pthread_create(&senders_id[i], NULL, sender, actor);
    }
   
    for (int i = 0; i < n_senders; i++) {
        pthread_join(senders_id[i], NULL);
    }

    int *msg_ext = (int *)calloc(1, sizeof(int));
    *msg_ext = -1;
    actor_send_to(actor, msg_ext);
    actor_finalize(actor);

    return 0;
}
