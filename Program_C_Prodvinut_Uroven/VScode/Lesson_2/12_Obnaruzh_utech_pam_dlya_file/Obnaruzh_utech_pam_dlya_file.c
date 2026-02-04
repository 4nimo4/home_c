/*
//------------------------------------------------------------------------------------
//        Обнаружение утечек памяти в файлах
//--------------------------------------------------------------------------------------
В выводе увидишь, что test2.txt остался «утечкой» — открыт, но не закрыт


Ключевые идеи (аналогия с malloc/free)
Параллель с твоим трекером памяти:
Было:
memlist — список list с полями address,size,comment.
Стало:
filelist — список file_nodeс полямиFILE*, имя файла, режим, место открытия.

Структура узла
typedef struct file_node {
    FILE *fp;                 // сам дескриптор файла
    char  mode[8];            // "r", "w", "rb" и т.п.
    char  filename[256];      // имя файла
    char  place[128];         // "имя_исходника:строка:функция"
    struct file_node *next;   // следующий в списке
} file_node;

Это полностью аналогично твоему typedef struct list { ... } list;, только для файлов.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//==================================================
//   Структура для отслеживания открытых файлов
//==================================================
typedef struct file_node {
    FILE *fp;                 // указатель на открытый файл
    char  mode[8];            // режим открытия ("r", "w", "rb" и т.п.)
    char  filename[256];      // имя файла
    char  place[128];         // где открыт: "file.c:строка:функция"
    struct file_node *next;   // следующий узел списка
} file_node;
/*
Глобальная голова списка
static file_node *filelist = NULL; Как static list *memlist = NULL;
, только для открытых файлов.
static — чтобы переменная была видна только в этом .c-файле.
*/
// Глобальная "голова" списка открытых файлов
static file_node *filelist = NULL;

//==================================================
//   Вставка нового узла (новый открытый файл)
//==================================================
/*
Вставка — file_insert Полный аналог твоего insert:
malloc(sizeof(file_node)); — создаём узел заполняем поля;
вставляем в начало списка (новый узел становится головой).
tmp->next = *head;
*head = tmp; — те же два шага, что и в списке памяти
*/
static void file_insert(file_node **head,
                        FILE *fp,
                        const char *filename,
                        const char *mode,
                        const char *place)
{
    file_node *tmp = malloc(sizeof(file_node));
    if (!tmp) {
        fprintf(stderr, "file_insert: cannot allocate node\n");
        return;
    }

    tmp->fp = fp;
    snprintf(tmp->mode, sizeof(tmp->mode), "%s", mode);
    snprintf(tmp->filename, sizeof(tmp->filename), "%s", filename);
    snprintf(tmp->place, sizeof(tmp->place), "%s", place);

    tmp->next = *head;   // новый узел в начало списка
    *head = tmp;
}

//==================================================
//   Удаление узла по FILE* (при fclose)
//==================================================
/*
Удаление — file_delete Полный аналог delete:
проверяем голову; если не голова — идём по списку, смотрим tmp->next->fp;
при совпадении fp вырезаем узел и free(del);.
Возвращаем _Bool 1 — нашли и удалили;0 — не нашли (подозрительный fclose).
*/
static _Bool file_delete(file_node **head, FILE *fp)
{
    if (*head == NULL) {
        return 0;  // список пуст
    }

    file_node *del = NULL;

    // Проверяем голову
    if ((*head)->fp == fp) {
        del = *head;
        *head = (*head)->next;
        free(del);
        return 1;
    }

    // Ищем в середине/хвосте
    file_node *tmp = *head;
    while (tmp->next) {
        if (tmp->next->fp == fp) {
            del = tmp->next;
            tmp->next = del->next;
            free(del);
            return 1;
        }
        tmp = tmp->next;
    }

    // Не нашли такой FILE* в списке
    return 0;
}

//==================================================
//   Обёртка над fclose: удаляет запись из списка
//==================================================
/*
my_fclose: 
вызывает настоящий fclose; удаляет запись из filelist через file_delete;
если не нашёл — пишет WARNING с файлом/строкой.
*/
int my_fclose(FILE *fp, const char *file, int line, const char *func)
{
    // Сначала реальный fclose
    int rc = fclose(fp);

    // Затем удаляем из списка
    if (!file_delete(&filelist, fp)) {
        fprintf(stderr,
                "WARNING: fclose of unknown FILE*: %p (%s:%d, %s)\n",
                (void*)fp, file, line, func);
    }

    return rc;  // возвращаем код из настоящего fclose
}

//==================================================
//   Обёртка над fopen: добавляет запись в список
//==================================================
/*
my_fopen:
вызывает настоящий fopen; если успех — добавляет запись в filelist через file_insert.
*/
FILE* my_fopen(const char *filename,
               const char *mode,
               const char *file,
               int line,
               const char *func)
{
    FILE *fp = fopen(filename, mode);  // настоящий fopen
    if (!fp) {
        // Открыть не удалось — сразу возвращаем NULL
        fprintf(stderr,
                "my_fopen: cannot open '%s' with mode '%s' (%s:%d, %s)\n",
                filename, mode, file, line, func);
        return NULL;
    }

    char place[128] = {0};
    // Место, где файл был открыт: "имя_файла:строка:функция"
    snprintf(place, sizeof(place), "%s:%d:%s", file, line, func);

    // Запоминаем этот FILE* в списке
    file_insert(&filelist, fp, filename, mode, place);

    return fp;
}

//==================================================
//   Макросы-обёртки для удобного использования
//==================================================
/*
Обёртки — my_fopen/my_fclose + макросы По аналогии с:
#define malloc(X) my_malloc((X), __FILE__, __LINE__, __func__)
#define free(X)   my_free((X), __FILE__, __LINE__, __func__)

мы сделали:
#define fopen(name, mode) my_fopen((name), (mode), __FILE__, __LINE__, __func__)
#define fclose(fp)        my_fclose((fp),  __FILE__, __LINE__, __func__)
То есть: 
каждый fopen(...) реально становится my_fopen("test1.txt", "w", "исходник.c", 123, "main");
каждый fclose(fp) — my_fclose(fp, "исходник.c", 130, "main");
*/
#define fopen(name, mode) my_fopen((name), (mode), __FILE__, __LINE__, __func__)
#define fclose(fp)        my_fclose((fp),  __FILE__, __LINE__, __func__)

//==================================================
//   Печать списка открытых файлов (утечки файлов)
//==================================================
void print_open_files(void)
{
    if (filelist == NULL) {
        printf("No open files detected\n");
        return;
    }

    printf("Open files detected (possible leaks):\n");
    file_node *cur = filelist;
    while (cur) {
        printf("FILE*=%p name='%s' mode='%s' opened at %s\n",
               (void*)cur->fp, cur->filename, cur->mode, cur->place);
        cur = cur->next;
    }
}

//==================================================
//   Пример использования
//==================================================
int main(void)
{
    FILE *f1 = fopen("test1.txt", "w");
    FILE *f2 = fopen("test2.txt", "w");

    if (f1) fprintf(f1, "Hello from f1\n");
    if (f2) fprintf(f2, "Hello from f2\n");

    // Закроем только один файл
    if (f1) fclose(f1);

    // В конце программы смотрим, какие файлы остались открытыми
    print_open_files();

    return 0;
}
/*

3. Что можно дальше разобрать детально
Можем пройти по этой системе так же, как мы делали:

Разобрать
typedef struct file_node { ... } file_node; — тип, указатель, head‑переменная.

Построчно:
static file_node *filelist = NULL;
Разобрать аналог
insert — file_insert.

Разобрать аналог
delete — file_delete
(там те же приёмы: tmp,del,tmp->next = del->next).

Затем — my_fopen и my_fclose и их связь с макросами.

*/