#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define DATA_SIZE 32
#define MAX_SIZE 128

typedef struct element {
    struct element* next;
    char* data[DATA_SIZE];
} element;

typedef struct TList
{
    element* first;
    int max_size;
    pthread_mutex_t mt;
    sem_t sem_free_slots;
} TList;

TList* createList (int size) {
    if(size < 1) return NULL;
    TList* list = (TList*)malloc(sizeof(TList));
    sem_init(&list->sem_free_slots, 0, size);
    pthread_mutex_init(&list->mt, NULL);
    pthread_mutex_lock(&list->mt);
    list->max_size = MAX_SIZE;
    element *current = (element*)malloc(sizeof(element));
    if(!current) {
        pthread_mutex_unlock(&list->mt);
        return NULL;
    }
    list->first = current;

    for(int i = 0; i < DATA_SIZE; i++) {
        current->data[i] = NULL;
    }
    for (int i = 0; i < size - 1; i++) {
        current->next = (element *)malloc(sizeof(element));
        if(!current->next) {
            return NULL;
        }
        current = current->next;
        for (int j = 0; j < DATA_SIZE; j++) {
            current->data[j] = NULL;
        }
    }

    current->next = NULL;
    pthread_mutex_unlock(&list->mt);
    return list;
};

int getCount (TList* list) {
    if (!list || !list->first) return 0;

    int count = 0;
    element* current = list->first;
    while(current->next != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void setMaxSize (TList* list, int n) {
    if(n < 1) {
        return;
    }
    pthread_mutex_lock(&list->mt);
    int current_size = list->max_size;
    list->max_size = n;

    if(n>current_size) {
        for(int i = n; i < n - current_size; i++) {
            sem_post(list->sem_free_slots);
        }
    }
    else if(n < current_size) {
        for(int i = n; i < current_size - n; i++) {
            int sem_value;
            sem_getvalue(&list->sem_free_slots, &sem_value);
            if(sem_value > 0) {
                sem_wait(list->sem_free_slots);
            }
        }
    }
    pthread_mutex_unlock(&list->mt);
}

int putItem (TList* list, char* item[DATA_SIZE]) {
    sem_wait(&list->sem_free_slots);
    pthread_mutex_lock(&list->mt);

    element* new_element = (element*)malloc(sizeof(element));
    if(!new_element) {
        pthread_mutex_unlock(&list->mt);
        sem_post(&list->sem_free_slots);
        return -1;
    }
    for (int i = 0; i < DATA_SIZE; i++) {
        if (item[i] != NULL) {
            new_element->data[i] = strdup(item[i]);
            if (!new_element->data[i]) {
                for (int j = 0; j < i; j++) {
                    free(new_element->data[j]);
                }
                free(new_element);
                pthread_mutex_unlock(&list->mt);
                return -1;
            }
        } else {
            new_element->data[i] = NULL;
        }
    }
    element* current = list->first;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = new_element;

    pthread_mutex_unlock(&list->mt);
    return 0;
}

char** getItem(TList *list) {
    pthread_mutex_lock(&list->mt);
    element* to_remove = list->first;
    if(to_remove == NULL) {
        pthread_mutex_unlock(&list->mt);
        return NULL;
    }
    list->first = to_remove->next;
    char** data_to_return = to_remove->data;
    free(to_remove);
    pthread_mutex_unlock(&list->mt);
    return data_to_return;
}

int removeItem (TList* list, char* item[DATA_SIZE]) {
    pthread_mutex_lock(&list->mt);
    if(list->first == NULL) {
        pthread_mutex_unlock(&list->mt);
        return -1;
    }
    element* current = list->first;
    element *previous = NULL;
    while(current->next != NULL) {
        if(current->data[0] == item[0]) {
            if(previous == NULL) {
                list->first = current->next;
            }
            else {
                previous->next = current->next;
            }

            for(int i = 0; i < DATA_SIZE; i++) {
                free(current->data[i]);
            }
            free(current);
            pthread_mutex_unlock(&list->mt);
            sem_post(&list->sem_free_slots);
            return 0;
        }
        previous = current;
        current = current->next;
    }
    pthread_mutex_unlock(&list->mt);
    return -1;
}

void destroyList(TList* list) {
    pthread_mutex_lock(&list->mt);

    element *current = list->first;

    while(current) {
        element* temp_el = current;
        current = current->next;
        for(int i = 0; i < DATA_SIZE; i++) {
            free(temp_el->data[i]);
        }
        free(temp_el);
    }
    list->first = NULL;
    free(list);
    pthread_mutex_unlock(&list->mt);
}

int appendItems(TList* list, TList* list2) {
    pthread_mutex_lock(&list->mt);
    pthread_mutex_lock(&list2->mt);
    for(int i = 0; i < getCount(list2); i++) {
        char** item = getItem(list2);
        if(item == NULL) {
            pthread_mutex_unlock(&list->mt);
            pthread_mutex_unlock(&list2->mt);
            return -1;
        }
        if(putItem(list, item) == 0) {
            sem_wait(list->sem_free_slots);
            sem_post(list2->sem_free_slots);
        }
        else {
            pthread_mutex_unlock(&list->mt);
            pthread_mutex_unlock(&list2->mt);
            return -1;
        }
    }
    list2->first = NULL;
    pthread_mutex_unlock(&list->mt);
    pthread_mutex_unlock(&list2->mt);
    return 0;
}

void showList(TList* list) {
    pthread_mutex_lock(&list->mt);
    element* current = list->first;
    while(current != NULL) {
        for (int i = 0; i < DATA_SIZE; i++) {
            if(current->data[i] != NULL) {
                printf("%s\t", current->data[i]);
            }
            else {
                printf("NULL\t");
            }

        }
        printf("\n");
        current = current->next;
    }
    pthread_mutex_unlock(&list->mt);
}

int main() {
    return 0;
}
