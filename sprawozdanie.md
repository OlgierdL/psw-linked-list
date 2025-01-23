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
      int max_size;
      pthread_mutex_t mt;
      pthread_cond_t cond_not_empty;
      pthread_cond_t cond_not_full;
    };
   ```
   Zawiera ona wskaźnik na głowę listy ```first```, zmienną ```max_size```, zamek ```mt``` oraz zmienne warunkowe ```cond_not_empty``` i `cond_not_full` służące do kontrolowania dodawania i usuwania elementów.


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
3. `void putItem(TList *lst, int *itm)` -- umieszczenie nowego elementu zawierającego dane `itm` na końcu listy `lst`.
4. `void *getItem(TList *lst)` -- pobranie z listy `lst` pierwszego elementu (usunięcie go z listy zwracając wskaźnik).
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
```Basic tests:

Created list of size 5
Initial list:
Putting 0 in list.
0
Created p/c threads
Getting 0 from list.
Putting 1 in list.
1
Putting 2 in list.
1
2
Getting 1 from list.
2
Putting 3 in list.
2
3
Putting 4 in list.
2
3
4
Getting 2 from list.
3
4
Putting 5 in list.
3
4
5
Putting 6 in list.
3
4
5
6
Getting 3 from list.
4
5
6
Putting 7 in list.
4
5
6
7
Putting 8 in list.
4
5
6
7
8
Getting 4 from list.
5
6
7
8
Putting 9 in list.
5
6
7
8
9
Getting 5 from list.
6
7
8
9
Getting 6 from list.
7
8
9
Getting 7 from list.
8
9
Getting 8 from list.
9
Getting 9 from list.
Finished p/c threads
Created list of size 3
Created extra list:
Putting 10 in list.
Putting 11 in list.
Putting 12 in list.
Extra list before appending:
10
11
12
Putting 1 in list.
Appending items to list:
1
List being apppended:
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
Found item to remove.
Successfully removed 1546304818 from the list
List after removing:
1
11
12
Advanced tests:

Created list of size 2
Putting 98 in list.
Putting 99 in list.
Put two items in the list:
98
99
Test: Adjusting Max Size
Initial list:
98
99
Increasing max size to 10.
Filling list after increasing max size.
Putting 100 in list.
Putting 101 in list.
Putting 102 in list.
Putting 103 in list.
Putting 104 in list.
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
Getting 98 from list.
Getting 99 from list.
Removed two items.
Putting 0 in list.
100
101
102
103
104
0
Putting 1 in list.
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
Test: Edge Cases
Created list of size 2
Testing adding to an empty list:
Putting 42 in list.
42
Filling the list to its max capacity:
Putting 84 in list.
42```
