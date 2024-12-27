#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define DATA_SIZE 32

pthread_mutex_t mt;

typedef struct TList
{
    struct TList* next;
    char* data[DATA_SIZE];
} TList;

TList* createList (int size) {
    if(size < 1) return NULL;

    pthread_mutex_lock(&mt);

    TList *first = (TList *)malloc(sizeof(TList));
    if(!first) return NULL;

    TList* current = first;
    for(int i = 0; i < DATA_SIZE; i++) {
        current->data[i] = NULL;
    }

    for (int i = 0; i < size - 1; i++) {
        current->next = (TList *)malloc(sizeof(TList));
        if(!current->next) {
            //destroyList(first);
            return NULL;
        }
        current = current->next;
        for (int j = 0; j < DATA_SIZE; j++) {
            current->data[j] = NULL;
        }
    }

    current->next = NULL;
    pthread_mutex_unlock(&mt);
    return first;
};

int main() {
    pthread_mutex_init(&mt, NULL);
    return 0;
}
