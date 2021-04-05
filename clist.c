#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "clist.h"

struct clist {
    size_t insertions;
    size_t capacity;
    size_t item_sz;
    void *element_storage;
};

struct clist_iterator {
    unsigned int initialized : 1;
    size_t idx;
};

struct clist *clist_create(size_t capacity, size_t item_sz)
{
    struct clist *list =  malloc(sizeof(struct clist));
    if (list == NULL) {
        free(list);
        perror("Could not malloc");
        return  NULL;
    }

    if (capacity == 0) {
        free(list);
        return NULL;
    }

    list->item_sz = item_sz;
    list->capacity = capacity;
    list->insertions = 0;

    list->element_storage = malloc (list->capacity * list->item_sz);
    if (list->element_storage == NULL) {
        perror("malloc error");
	    free(list);
	    return NULL;
    }
    return list;

}

void clist_destroy(struct clist *list)
{
    if (list != NULL) {
        free(list->element_storage);
        free(list);
    }
}

struct clist_iterator clist_create_iter(void)
{
    struct clist_iterator it = { 0 };
    return it;
}

ssize_t clist_add(struct clist *list, void *item) {
    if (list == NULL) {
        return -1;
    }
    size_t idx = list->insertions % list->capacity;
    void *ptr = list->element_storage + idx * list->item_sz;
    memcpy(ptr, item, list->item_sz);
    return list->insertions++;
}

void *clist_add_new(struct clist *list)
{
    if (list == NULL) {
        return NULL;
    }
    int idx = list->insertions % list->capacity;
    void *ptr = list->element_storage + idx * list->item_sz;
    list->insertions++;
    return ptr;
}

void *clist_get(struct clist *list, size_t idx) {
    if (list == NULL || idx >= list->insertions || ((list->insertions > list->capacity) && (idx < list->insertions - list->capacity)))
    {
        return NULL;
    } 
    // mod idx
    size_t real_idx = idx % list->capacity;

    // first we goes from element storage
    // increment on element storage
    return list->element_storage + real_idx * list->item_sz;
}

void *clist_iterate(struct clist *list, struct clist_iterator *iter) {
    void *ptr = clist_get(list, (list->insertions - iter->idx - 1));
    iter->idx++;
    return ptr;
}

void *clist_iterate_rev(struct clist *list, struct clist_iterator *iter) {
    if (list->insertions < list->capacity) {
        return clist_get(list, iter->idx++);
    } else {
        void *ptr = clist_get(list, iter->idx + list->insertions - list->capacity);
        iter->idx++;
        return ptr;
    }
}

int main(void)
{
    struct clist *list = clist_create(5, sizeof(int));

    int x;
    x = 0;
    clist_add(list, &x);

    x = 1; clist_add(list, &x);
    x = 2; clist_add(list, &x);
    x = 3; clist_add(list, &x);
    x = 4; clist_add(list, &x);
    x = 5; clist_add(list, &x);
    x = 6; clist_add(list, &x);

    int *y = clist_get(list, 2);
    if (y == NULL) {
        clist_destroy(list);
        return 1;
    }
    printf("y = %d\n\n", *y);

    void *elem;
    struct clist_iterator iter = clist_create_iter();
    while ((elem = clist_iterate(list, &iter)) != NULL) {
        int ie = *((int *) elem);
        printf("-> %d\n", ie);
    }

    puts("\n\n");

    struct clist_iterator iter2 = clist_create_iter();
    while ((elem = clist_iterate_rev(list, &iter2)) != NULL) {
        int ie = *((int *) elem);
        printf("-> %d\n", ie);
    }

    clist_destroy(list);

    return 0;
}
