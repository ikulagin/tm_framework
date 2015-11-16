
typedef struct list_s list_t;

list_t *list_alloc();
__attribute__((transaction_safe))
bool list_insert(list_t *list_ptr, void *data, size_t data_size);

void list_print(list_t *list_ptr);
long list_total_size(list_t *list_ptr);

__attribute__((transaction_safe))
void list_free(list_t *list_ptr);
