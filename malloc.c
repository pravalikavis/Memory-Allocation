

#include "stdafx.h"
#include "heapmanager.h"
#include <stdlib.h>

#include <stdio.h>

struct meta_data
{
    int start = 0;
    int stop = 1023;
    struct meta_data *next = NULL;
};
struct heap_node
{
    void *data = NULL;
    struct meta_data *meta_head = NULL;
    struct heap_node *next = NULL;
};

struct heap_node *create_nodes()
{
    struct heap_node *heap = (struct heap_node *)calloc(1, sizeof(struct heap_node));
    heap->data = calloc(1024, 1);
    struct meta_data *meta = (struct meta_data *)calloc(1, sizeof(struct meta_data));
    meta->start = 0;
    meta->stop = 1023;
    heap->meta_head = meta;
    return heap;
}
int allocate(struct heap_node *n, int size)
{
    struct meta_data *m = n->meta_head;
    if (m->start == m->stop)
    {
        return -1;
    }
    struct meta_data *temp = m;
    while (m != NULL)
    {
        if (m->stop - m->start + 1 > size)
        {
            int pointer = m->start;
            m->start += size;
            if (m->start == m->stop)
            {
                temp->next = m->next;
                free(m);
            }

            return pointer;
        }
        temp = m;
        m = m->next;
    }
}

struct heap_node *allocate_if_heap_full(struct heap_node *m)

{
    struct heap_node *t = m;
    if (m->meta_head->start == m->meta_head->stop)
    {
        if (m->next != NULL)
        {

            return m->next;
        }

        struct meta_data *meta = (struct meta_data *)calloc(1, sizeof(struct meta_data));
        struct heap_node *headnode = (struct heap_node *)calloc(1, sizeof(struct heap_node));
        headnode->data = calloc(1024, 1);
        headnode->next = NULL;
        headnode->meta_head = meta;
        return headnode;
    }
    return m;
}
void remove_if_heap_is_empty(struct heap_node *heap)
{
    if (heap->meta_head->start == 0 && heap->meta_head->stop == 1023)
        heap = NULL;
}

void merge(struct heap_node *head)
{
    struct meta_data *meta = head->meta_head;
    if (meta == NULL)
        return;
    struct meta_data *before = meta;
    struct meta_data *after;
    if (meta->next != NULL)
        meta = meta->next;
    else
        return;
    while (meta != NULL)
    {
        if (before->stop + 1 == meta->start)
        {
            after = meta->next;
            before->stop = meta->stop;
            before->next = after;
            meta = after;
            continue;
        }
        meta = meta->next;
    }
}
void free_the_memory(struct heap_node *heap, int start, int stop)
{
    struct meta_data *meta = heap->meta_head;
    if (stop < meta->start)
    {
        struct meta_data temp;
        temp.start = start;
        temp.stop = stop;
        temp.next = meta;
        heap->meta_head = &temp;
        return;
    }

    struct meta_data *temp = meta;
    meta = meta->next;
    while (meta != NULL)
    {
        if (start > temp->stop && stop < temp->start)
        {
            struct meta_data temp1;
            temp1.start = start;
            temp1.stop = stop;
            temp1.next = meta;
            temp->next = temp;
            return;
        }
    }
}

namespace heapmanager
{

struct heap_node *hn = NULL;
void *Functions::vlp_malloc(int size)
{
    if (hn == NULL)
    {
        hn = create_nodes();
    }
    struct heap_node *filling;
    //filling = allocate_if_heap_full(hn);
    int allocated = allocate(hn, size + 2);
    while (allocated == -1)
    {
        filling = allocate_if_heap_full(hn);

        allocated = allocate(hn, size + 2);
    }
    short int *put_size = (short int *)(((char *)hn->data) + allocated);
    *put_size = size;
    return (void *)(((char *)hn->data) + allocated + 2);
}

void Functions::vlp_free(void *free)
{

    short int *get_freed = (short int *)(void *)((char *)free - 2);
    int stop = *get_freed + 1;
    struct heap_node *heap = (struct heap_node *)calloc(1, sizeof(struct heap_node));
    heap = hn;
    while (heap != NULL)
    {
        struct meta_data *meta = heap->meta_head;
        int *a = (int *)free;
        int c = (size_t)a;
        int *d = (int *)(heap->data);
        int e = (size_t)d;
        int range = c - e - 2;
        if (range >= 0 && range <= 1023)
        {
            free_the_memory(heap, range, stop);
            struct meta_data *meta = heap->meta_head;
            if (meta == NULL)
                return;
            struct meta_data *before = meta;
            struct meta_data *after;
            if (meta->next != NULL)
                meta = meta->next;
            else
                return;
            while (meta != NULL)
            {
                if (before->stop + 1 == meta->start)
                {
                    after = meta->next;
                    before->stop = meta->stop;
                    before->next = after;
                    meta = after;
                    continue;
                }
                meta = meta->next;
            }
        }
        heap = heap->next;
    }
}
void Functions::vlp_debug()
{
    struct heap_node *heap = hn;
    while (heap != NULL)
    {

        printf("%s", (char *)hn->data);
    }
}

} // namespace heapmanager