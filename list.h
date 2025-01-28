#ifndef LCL_LIST_H
#define LCL_LIST_H

// ==============================================
//
//  Version 1.0, 2025-01-15
//
// ==============================================
struct element {
    struct element* next;
    void* data;
};
typedef struct element element;

struct TList {
    element* first;
    element* last;
    int count;
    int max_size;
    int is_destroyed;
    pthread_mutex_t mt;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;
};
typedef struct TList TList;

TList *createList(int s);

void destroyList(TList *lst);

void putItem(TList *lst, void *itm);

void *getItem(TList *lst);

void* popItem(TList* lst);

int removeItem(TList *lst, void *itm);

int getCount(TList *lst);

void setMaxSize(TList *lst, int s);

void appendItems(TList *lst, TList *lst2);

void showList(TList *lst);

#endif //LCL_LIST_H
