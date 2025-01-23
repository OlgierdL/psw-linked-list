#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include "list.h"

TList* createList (int s) {
    if(s < 0) return NULL;
    TList* list = (TList*)malloc(sizeof(TList));
    pthread_mutex_init(&list->mt, NULL);
    pthread_mutex_lock(&list->mt);
    list->max_size = s;
    list->first = NULL;
    pthread_cond_init(&list->cond_not_empty, NULL);
    pthread_cond_init(&list->cond_not_full, NULL);
    pthread_mutex_unlock(&list->mt);
    printf("Created list of size %d\n", list->max_size);
    return list;
};

int getCount (TList* lst) {
    if (!lst || !lst->first) return 0;

    int count = 1;
    element* current = lst->first;
    while(current->next != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void setMaxSize (TList* lst, int s) {
    if(s < 0) {
        return;
    }
    pthread_mutex_lock(&lst->mt);
    int current_size = lst->max_size;
    lst->max_size = s;

    if(s>current_size) {
        pthread_cond_broadcast(&lst->cond_not_full);
    }
    else if(s < current_size && lst->first != NULL) {
        pthread_cond_broadcast(&lst->cond_not_empty);
    }
    pthread_mutex_unlock(&lst->mt);
}

void putItem (TList *lst, int *itm) {
    pthread_mutex_lock(&lst->mt);
    while (getCount(lst) >= lst->max_size) {
        pthread_cond_wait(&lst->cond_not_full, &lst->mt);
    }

    element* new_element = (element*)malloc(sizeof(element));
    if(!new_element) {
        pthread_mutex_unlock(&lst->mt);
        return;
    }
    new_element->data = malloc(sizeof(int));
    if (new_element->data) {
        *(new_element->data) = *itm;
    }
    if(lst->first != NULL) {
        element* current = lst->first;
        while(current->next != NULL) {
            current = current->next;
        }
        current->next = new_element;
        new_element->next = NULL;
    }
    else {
        lst->first = new_element;
        new_element->next = NULL;
    }
    pthread_cond_signal(&lst->cond_not_empty);
    printf("Putting %d in list.\n", *itm);
    pthread_mutex_unlock(&lst->mt);
}

void* getItem(TList *lst) {
    pthread_mutex_lock(&lst->mt);
    while (lst->first == NULL) {
        pthread_cond_wait(&lst->cond_not_empty, &lst->mt);
    }
    element* to_remove = lst->first;
    lst->first = to_remove->next;
    int* data_to_return = to_remove->data;
    free(to_remove);
    printf("Getting %d from list.\n" , *data_to_return);
    pthread_cond_signal(&lst->cond_not_full);
    pthread_mutex_unlock(&lst->mt);
    return data_to_return;
}

int removeItem (TList *lst, void *itm) {
    pthread_mutex_lock(&lst->mt);
    if(lst->first == NULL) {
        pthread_mutex_unlock(&lst->mt);
        return -1;
    }
    element* current = lst->first;
    element *previous = NULL;
    while(current != NULL) {
        if(current->data == itm) {
            printf("Found item to remove.\n");
            if(previous == NULL) {
                lst->first = current->next;
            }
            else {
                previous->next = current->next;
            }
            free(current->data);
            free(current);
            pthread_cond_signal(&lst->cond_not_full);
            pthread_mutex_unlock(&lst->mt);
            return 0;
        }
        previous = current;
        current = current->next;
    }
    pthread_mutex_unlock(&lst->mt);
    return -1;
}

void destroyList(TList* lst) {
    pthread_mutex_lock(&lst->mt);

    element *current = lst->first;

    while(current) {
        element* temp_el = current;
        current = current->next;
        free(temp_el->data);
        free(temp_el);
    }
    lst->first = NULL;
    pthread_mutex_unlock(&lst->mt);

    pthread_mutex_destroy(&lst->mt);
    pthread_cond_destroy(&lst->cond_not_empty);
    pthread_cond_destroy(&lst->cond_not_full);
    free(lst);
}

void appendItems(TList* lst, TList* lst2) {
    printf("Appending items to list:\n");
    showList(lst);
    printf("List being apppended:\n");
    showList(lst2);
    pthread_mutex_lock(&lst->mt);
    pthread_mutex_lock(&lst2->mt);
    while (lst2->first != NULL) {
        element* to_remove = lst2->first;

        lst2->first = to_remove->next;
        int* itm = to_remove->data;
        free(to_remove);
        pthread_cond_signal(&lst2->cond_not_full);

        while (getCount(lst) >= lst->max_size) {
            pthread_cond_wait(&lst->cond_not_full, &lst->mt);
        }

        element* new_element = (element*)malloc(sizeof(element));
        if(!new_element) {
            printf("Failed to create new element.");
            pthread_mutex_unlock(&lst2->mt);
            pthread_mutex_unlock(&lst->mt);
            return;
        }
        new_element->data = itm;
        new_element->next = NULL;

        if(lst->first != NULL) {
            element* current = lst->first;
            while(current->next != NULL) {
                current = current->next;
            }
            current->next = new_element;
        }
        else {
            lst->first = new_element;
        }
        pthread_cond_signal(&lst->cond_not_empty);
    }

    pthread_mutex_unlock(&lst->mt);
    pthread_mutex_unlock(&lst2->mt);
}

void showList(TList* lst) {
    if(lst == NULL) {
        return;
    }
    pthread_mutex_lock(&lst->mt);
    if(lst->first == NULL) {
        pthread_mutex_unlock(&lst->mt);
        return;
    }
    element* current = lst->first;
    while(current != NULL) {
        if(current->data != NULL) {
            printf("%d\t", *current->data);
        }
        else {
            printf("NULL\t");
        }
        printf("\n");
        current = current->next;
    }
    pthread_mutex_unlock(&lst->mt);
}


