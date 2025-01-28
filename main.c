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
        printf("Putting data in list.\n");
        showList(list);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *small_producer(void *arg) {
    TList *list = (TList *)arg;
    for (int i = 0; i < 2; ++i) {
        int *data = malloc(sizeof(int));
        *data = i;
        putItem(list, data);
        printf("Putting data in list.\n");
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
            printf("Getting data from list.\n");
            free(data);
            showList(list);
        } else {
            printf("Consumer: List is empty\n");
        }
        sleep(2);
    }
    pthread_exit(NULL);
}

void testSetMaxSize(TList *list) {
    printf("Test: Adjusting Max Size\n");
    printf("Initial list:\n");
    showList(list);

    printf("Increasing max size to 10.\n");
    setMaxSize(list, 10);

    printf("Filling list after increasing max size.\n");
    for (int i = 0; i < 5; ++i) {
        int *data = malloc(sizeof(int));
        *data = 100 + i;
        putItem(list, data);
    }
    showList(list);

    printf("Decreasing max size to 7.\n");
    setMaxSize(list, 7);
    printf("List after decreasing max size:\n");
    showList(list);

    printf("Trying to add more items after decreasing max size.\n");
    pthread_t prod_thread;
    pthread_create(&prod_thread, NULL, small_producer, list);
    int* data1 = getItem(list);
    free(data1);
    int* data2 = getItem(list);
    free(data2);
    printf("Removed two items.\n");
    pthread_join(prod_thread, NULL);

    printf("Final list after testing max size adjustments:\n");
    showList(list);
}

void testEdgeCases() {
    printf("Test: Edge Cases\n");
    TList *list = createList(2);
    printf("Testing adding to an empty list:\n");
    int *data1 = malloc(sizeof(int));
    *data1 = 42;
    putItem(list, data1);
    showList(list);

    printf("Filling the list to its max capacity:\n");
    int *data2 = malloc(sizeof(int));
    *data2 = 84;
    putItem(list, data2);
    showList(list);
    destroyList(list);
}

int main() {
    printf("Basic tests:\n\n");
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

    printf("Advanced tests:\n\n");

    TList *list2 = createList(3);
    int *itm1 = malloc(sizeof(int));
    int *itm2 = malloc(sizeof(int));
    int *itm3 = malloc(sizeof(int));
    *itm1 = 98;
    putItem(list2, itm1);
    *itm2 = 99;
    putItem(list2, itm2);
    *itm3 = 100;
    putItem(list2, itm3);
    removeItem(list2, itm2);
    setMaxSize(list2, 2);
    printf("Put two items in the list:\n");
    showList(list2);
    testSetMaxSize(list2);
    printf("Popping:\n");
    popItem(list2);
    showList(list2);
    destroyList(list2);

    testEdgeCases();

    return 0;
}
