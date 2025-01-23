#ifndef LCL_LIST_H
#define LCL_LIST_H

// ==============================================
//
//  Version 1.0, 2025-01-15
//
// ==============================================
struct element {
    struct element* next;
    int* data;
};
typedef struct element element;

struct TList {
    element* first;
    int max_size;
    pthread_mutex_t mt;
    sem_t sem_free_slots;
};
typedef struct TList TList;

TList *createList(int s);

void destroyList(TList *lst);

void putItem(TList *lst, int *itm);

void *getItem(TList *lst);

void *popItem(TList *lst);

int removeItem(TList *lst, void *itm);

int getCount(TList *lst);

void setMaxSize(TList *lst, int s);

void appendItems(TList *lst, TList *lst2);

void showList(TList *lst);

#endif //LCL_LIST_H
