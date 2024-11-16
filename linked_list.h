//
// Created by Alex Brodsky on 2023-09-20.
//

#ifndef LINKED_LIST__H
#define LINKED_LIST__H

typedef struct ll_node {
    struct ll_node *prev;
    struct ll_node *next;
    void *item;
} ll_node_t;

typedef struct linked_list {
    ll_node_t *head;
    ll_node_t *tail;
} linked_list_t;

extern linked_list_t * ll_new();
extern void ll_destroy(linked_list_t *list);
extern int ll_add_front(linked_list_t *list, void *item);
extern int ll_add_back(linked_list_t *list, void *item);
extern void * ll_get_front(linked_list_t *list);
extern void * ll_get_back(linked_list_t *list);

extern void * ll_remove_front(linked_list_t *list);
extern void * ll_remove_back(linked_list_t *list);

/* To be added */
extern int ll_size(linked_list_t *list);
extern int ll_is_empty(linked_list_t *list);
extern int ll_insert(linked_list_t *list, ll_node_t *node, void *item);
extern void * ll_remove(linked_list_t *list, ll_node_t *node);


#endif //LINKED_LIST__H
