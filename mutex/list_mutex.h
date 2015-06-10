
typedef struct list_s list_t;

list_t *list_alloc();
bool list_insert(list_t *list_ptr, void *data, size_t data_size);

void list_print(list_t *list_ptr);
long list_total_size(list_t *list_ptr);

void list_free(list_t *list_ptr);
