#ifndef LF_LIST_H
#define LF_LIST_H

typedef struct lf_list_s lf_list_t;
typedef struct lf_list_node_s lf_list_node_t;

enum state_e {
    DATA = 0,
    DEAD = 1,
    INSERT = 2,
    REMOVE = 3,
};

lf_list_node_t *lf_list_node_create(int s, void *data, size_t data_size);
int lf_list_node_cas_state(lf_list_node_t *node, int state_orig, int state_new);
int lf_list_node_cas_next(lf_list_node_t *node, lf_list_node_t *next_orig,
                          lf_list_node_t *next_new);
void lf_list_node_delete(lf_list_node_t *node);

lf_list_t *lf_list_create();
int lf_list_insert(lf_list_t *list, lf_list_node_t *node);
long lf_list_total_size(lf_list_t *list);
void lf_list_delete(lf_list_t *list);

#endif
