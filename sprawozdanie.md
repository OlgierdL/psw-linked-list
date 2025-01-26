---
title:    Lista
subtitle: Programowanie systemowe i współbieżne
author:   Olgierd Ludwiczak 160266
date:     v1.0, 2025-23-01
lang:     pl-PL
---


Projekt jest dostępny w repozytorium pod adresem:  
<https://github.com/OlgierdL/psw-linked-list>.


# Struktury danych

Stosowane są dwie struktury: `TList` oraz `element`.

1. Lista definiowana jest strukturą `TList`:

   ```C
    struct TList {
      element* first;
      element* last;
      int count;
      int max_size;
      pthread_mutex_t mt;
      pthread_cond_t cond_not_empty;
      pthread_cond_t cond_not_full;
    };
   ```
   Zawiera ona wskaźnik na głowę listy `first`, ostatni element `last`, zmienne `max_size` i `count`, zamek `mt` oraz zmienne warunkowe `cond_not_empty` i `cond_not_full` służące do kontrolowania dodawania i usuwania elementów.


2. Element listy definiowany jest strukturą `element`:

   ```C
   struct element {
    struct element* next;
    int* data;
    };
   ```
   Zawiera ona wskaźnik na następny element `next` oraz wskaźnik na dane `data`.

# Funkcje

1. `TList *createList(int s)` -- stworzenie nowej struktury `TList` o możliwym maksymalnym rozmiarze `s`.
2. `void destroyList(TList *lst)` -- usunięcie listy `lst` i zwolnienie pamięci.
3. `void putItem(TList *lst, void *itm)` -- umieszczenie nowego elementu zawierającego dane `itm` na końcu listy `lst`.
4. `void *getItem(TList *lst)` -- pobranie z listy `lst` pierwszego elementu (usunięcie go z listy zwracając wskaźnik).
5. `void* popItem(TList* lst)` -- pobranie z listy `lst` ostatniego elementu (usunięcie go z listy zwracając wskaźnik).
5. `int removeItem(TList *lst, void *itm)` -- usunięcie z listy `lst` elementu zawierającego wskaźnik `itm`.
6. `int getCount(TList *lst)` -- zwraca liczbę elementów listy `lst`.
7. `void setMaxSize(TList *lst, int s)` -- ustawia nowy maksymalny rozmiar listy `lst`. Nie usuwa elementów o przekraczających go indeksach, ale uniemożliwia dodanie nowych danych do czasu odpowiedniego zmniejszenia długości listy.
8. `void appendItems(TList *lst, TList *lst2)` -- dodaje elementy listy `lst2` (które z niej jednocześnie usuwa) do listy `lst`.
9. `void showList(TList *lst)` -- wyświetla w konsoli listę `lst`.


# Algorytm

Do zaimplementowania obsługi współbieżności w operacjach na liście zastosowano zamek i dwie zmienne warunkowe. Jedna z nich sygnalizuje, że lista nie jest pełna, druga sygnalizuje że nie jest ona pusta.
Zastosowanie zmiennych warunkowych zamiast, na przykład, semafora wielowartościowego zabezpiecza algorytm przed głodzeniem wątków. Weryfikacja ilości miejsca lub pustości listy (oczekiwanie na odpowiedni sygnał) ma miejsce w funkcji while, w której długość listy porównywana jest z jej maksymalnym rozmiarem. Takie rozwiązanie eliminuje aktywne oczekiwanie. Jedyne miejsce w kodzie, gdzie występuje opuszczenie dwóch zamków, to funkcja append items. Żadna funkcja operująca na mutexach nie wywołuje innej takiej funkcji. Nie powinny zatem wystąpić zakleszczenia, chyba że w przypadku próby pobierania elemantu z pustej listy, do której żaden producent nic nie dodaje. Kod nie zakłada wystąpienia takiego scenariusza, rozwiązaniem byłby timeout.

# Przykład użycia

Wywołanie:
```
gcc main.c list.c -o program -lpthread -Wall
./program
```
Wynik:
```
Basic tests:

Created list of size 5
Initial list:
Created p/c threads
Putting data in list.
0
Getting data from list.
1
Putting data in list.
1
Putting data in list.
1
2
Getting data from list.
2
Putting data in list.
2
3
Putting data in list.
2
3
4
Getting data from list.
3
4
Putting data in list.
3
4
5
Putting data in list.
3
4
5
6
Getting data from list.
4
5
6
Putting data in list.
4
5
6
7
Putting data in list.
4
5
6
7
8
Getting data from list.
5
6
7
8
Putting data in list.
5
6
7
8
9
Getting data from list.
6
7
8
9
Getting data from list.
7
8
9
Getting data from list.
8
9
Getting data from list.
9
Getting data from list.
Finished p/c threads
Created list of size 3
Created extra list:
Extra list before appending:
10
11
12
List after appending extra list:
1
10
11
12
List before removing:
1
10
11
12
Successfully removed 352677648 from the list
List after removing:
1
11
12
Advanced tests:

Created list of size 2
Put two items in the list:
98
99
Test: Adjusting Max Size
Initial list:
98
99
Increasing max size to 10.
Filling list after increasing max size.
98
99
100
101
102
103
104
Decreasing max size to 7.
List after decreasing max size:
98
99
100
101
102
103
104
Trying to add more items after decreasing max size.
Removed two items.
Putting data in list.
100
101
102
103
104
0
Putting data in list.
100
101
102
103
104
0
1
Final list after testing max size adjustments:
100
101
102
103
104
0
1
Popping:
100
101
102
103
104
0
Test: Edge Cases
Created list of size 2
Testing adding to an empty list:
42
Filling the list to its max capacity:
42
84
```
