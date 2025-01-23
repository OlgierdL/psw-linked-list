#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include "list.h"

void *producer(void *arg) {
    TList *list = (TList *)arg;
    for (int i = 0; i < 10; ++i) {
        int *data = malloc(sizeof(int));
        *data = i;
        putItem(list, data);
        showList(list);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    TList *list = (TList *)arg;
    sleep(1);
    for (int i = 0; i < 10; ++i) {
        int *data = (int *)getItem(list);
        if (data) {
            free(data);
            showList(list);
        } else {
            printf("Consumer: List is empty\n");
        }
        sleep(2);
    }
    pthread_exit(NULL);
}

int main() {
    TList *list = createList(5);
    printf("Initial list:\n");
    showList(list);

    pthread_t prod_thread, cons_thread;
    pthread_create(&prod_thread, NULL, producer, list);
    pthread_create(&cons_thread, NULL, consumer, list);
    printf("Created p/c threads\n");

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);
    printf("Finished p/c threads\n");

    TList *extra_list = createList(3);
    printf("Created extra list:\n");
    showList(extra_list);
    for (int i = 10; i < 13; ++i) {
        int *data = malloc(sizeof(int));
        *data = i;
        putItem(extra_list, data);
    }

    printf("Extra list before appending:\n");
    showList(extra_list);
    int *itm = malloc(sizeof(int));
    *itm = 1;
    putItem(list, itm);
    appendItems(list, extra_list);

    printf("List after appending extra list:\n");
    showList(list);

    int *target = list->first->next->data;
    printf("List before removing:\n");
    showList(list);
    int result = removeItem(list, target);
    if (result == 0) {
        printf("Successfully removed %d from the list\n", *target);
    } else {
        printf("Item %d not found in the list\n", *target);
    }
    printf("List after removing:\n");
    showList(list);

    destroyList(list);
    destroyList(extra_list);

    return 0;
}