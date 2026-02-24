/*
//------------------------------------------------------
//  D0-Отсортировать слова
//------------------------------------------------------ 
Дана строка, состоящая из английских букв и пробелов. 
В конце строки символ точка. Все слова разделены одним пробелом. 
Необходимо составить из слов односвязный список и упорядочить по алфавиту. 
Список необходимо удалить в конце программы. 
Для сравнение строк можно использовать strcmp. 
Необходимо использовать данную структуры организации списка.
struct list {
char word[20];
struct list *next;
}
Необходимо реализовать односвязный список и обслуживающие его функции.
add_to_list
swap_elements
print_list
delete_list
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 1024

struct list {
    char word[20];        // слово (максимум 19 символов + '\0')
    struct list *next;    // указатель на следующий элемент списка
};

//------------------------------------------------------
// Добавление слова в конец списка (head может быть NULL)
//------------------------------------------------------
void add_to_list(struct list **head, const char *word)
{
    struct list *new_node = (struct list*)calloc(1, sizeof(struct list));
    if (!new_node) 
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    // безопасно копируем слово в поле структуры
    strncpy(new_node->word, word, sizeof(new_node->word) - 1);
    new_node->word[sizeof(new_node->word) - 1] = '\0';
    new_node->next = NULL;

    if (*head == NULL) 
    {
        // список пуст — новый элемент становится головой
        *head = new_node;
    } 
    else 
    {
        // идём в конец и цепляем туда новый элемент
        struct list *p = *head;
        while (p->next != NULL)
            p = p->next;
        p->next = new_node;
    }
}

//------------------------------------------------------
// Обмен "содержимым" двух узлов списка (обмен слов)
// (Указатели на узлы не меняем, меняем только их word.)
//------------------------------------------------------
void swap_elements(struct list *a, struct list *b)
{
    if (!a || !b) 
        return;

    char tmp[20];
    strcpy(tmp, a->word);
    strcpy(a->word, b->word);
    strcpy(b->word, tmp);
}

//------------------------------------------------------
// Печать списка слов
//------------------------------------------------------
void print_list(const struct list *head)
{
    const struct list *p = head;
    while (p) 
    {
        printf("%s", p->word);
        if (p->next)
            printf(" ");
        p = p->next;
    }
    printf("\n");
}

//------------------------------------------------------
// Удаление всего списка и освобождение памяти
//------------------------------------------------------
void delete_list(struct list **head)
{
    struct list *p = *head;
    while (p) 
    {
        struct list *next = p->next;
        free(p);
        p = next;
    }
    *head = NULL;
}

//------------------------------------------------------
// Сортировка списка по алфавиту (простая пузырьковая)
//------------------------------------------------------
void sort_list(struct list *head)
{
    if (!head) return;

    int swapped;
    do {
        swapped = 0;
        struct list *p = head;
        while (p->next) 
        {
            if (strcmp(p->word, p->next->word) > 0) 
            {
                swap_elements(p, p->next);
                swapped = 1;
            }
            p = p->next;
        }
    } while (swapped);
}

//------------------------------------------------------
// main
//------------------------------------------------------
int main(void)
{
    char line[MAX_BUF];

    //printf("Введите строку (английские буквы и пробелы, в конце точка):\n");
    if (!fgets(line, sizeof(line), stdin)) 
    {
        fprintf(stderr, "Input error\n");
        return 1;
    }

    // Обрежем всё после первой точки (включая саму точку)
    char *dot = strchr(line, '.');
    if (dot)
        *dot = '\0';

    // Построим список слов
    struct list *head = NULL;

    // strtok разбивает строку по пробелам
    char *token = strtok(line, " ");
    while (token) 
    {
        if (*token != '\0')      // защита от пустых "слов"
            add_to_list(&head, token);
        token = strtok(NULL, " ");
    }

    //printf("Список слов (как введён):\n");
    //print_list(head);

    // Сортируем список по алфавиту
    sort_list(head);

    //printf("Список слов после сортировки:\n");
    print_list(head);

    // Удаляем список
    delete_list(&head);

    return 0;
}