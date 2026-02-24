/*
//------------------------------------------------------
//  D1-Вид сверху
//------------------------------------------------------ 
В программе описана структура для хранения двоичного дерева:
typedef struct tree {
datatype key;
struct tree *left, *right;
} tree;
Необходимо реализовать функцию, которая печатает вид дерева сверху. 
Функция должна строго соответствовать прототипу:

void btUpView(tree *root)
*/

#include <stdio.h>
#include <stdlib.h>

// -----------------------------------------------------
// Структура узла бинарного дерева
// -----------------------------------------------------
typedef struct tree {
    int key;
    struct tree *left, *right;
} tree;

// -----------------------------------------------------
// Вставка в бинарное дерево поиска
// -----------------------------------------------------
tree* insert(tree *root, int key)
{
    if (!root) 
    {
        root = (tree*)calloc(1, sizeof(tree));
        if (!root) 
        {
            fprintf(stderr, "Memory allocation error\n");
            exit(EXIT_FAILURE);
        }
        root->key = key;
        return root;
    }

    if (key < root->key)
        root->left  = insert(root->left,  key);
    else if (key > root->key)
        root->right = insert(root->right, key);
    // при равенстве можно ничего не делать или вставлять вправо — по заданию не критично

    return root;
}

// -----------------------------------------------------
// Очередь для обхода в ширину (BFS)
// -----------------------------------------------------
typedef struct qnode {
    tree *node;
    int hd;          // горизонтальное расстояние от корня
} qnode;

typedef struct {
    qnode *data;
    int front, back, cap;
} queue;

void qinit(queue *q, int cap)
{
    q->data = (qnode*)malloc(cap * sizeof(qnode));
    if (!q->data) 
    {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    q->front = q->back = 0;
    q->cap = cap;
}

int qis_empty(queue *q)
{
    return q->front == q->back;
}

void qpush(queue *q, tree *node, int hd)
{
    if (q->back >= q->cap) 
    {
        fprintf(stderr, "Queue overflow\n");
        exit(EXIT_FAILURE);
    }
    q->data[q->back].node = node;
    q->data[q->back].hd   = hd;
    q->back++;
}

qnode qpop(queue *q)
{
    return q->data[q->front++];
}

// -----------------------------------------------------
// Структура для хранения первого узла по каждому hd
// -----------------------------------------------------
typedef struct {
    int used;  // 0 — нет элемента для этого hd, 1 — есть
    int key;   // ключ узла
} hd_entry;

// hd будем считать в диапазоне [-100; 100]
#define MAX_HD 201  // всего 201 возможное значение hd (-100..100)
#define SHIFT  100  // индекс = hd + SHIFT

// -----------------------------------------------------
// Функция печати вида дерева сверху (top view)
// -----------------------------------------------------
void btUpView(tree *root)
{
    if (!root)
        return;

    hd_entry hd_map[MAX_HD];

    // Обнуляем таблицу
    for (int i = 0; i < MAX_HD; ++i) {
        hd_map[i].used = 0;
        hd_map[i].key  = 0;
    }

    // BFS по дереву с учётом hd
    queue q;
    qinit(&q, 1000);          
    qpush(&q, root, 0);       // корень имеет hd = 0

    int min_hd = 0, max_hd = 0;

    while (!qis_empty(&q)) {
        qnode cur = qpop(&q);
        tree *node = cur.node;
        int hd = cur.hd;

        int idx = hd + SHIFT;
        if (idx >= 0 && idx < MAX_HD) {
            // если для этого hd ещё ничего не записано — записываем текущий узел
            if (!hd_map[idx].used) {
                hd_map[idx].used = 1;
                hd_map[idx].key  = node->key;
            }
        }

        if (hd < min_hd) min_hd = hd;
        if (hd > max_hd) max_hd = hd;

        if (node->left)
            qpush(&q, node->left, hd - 1);
        if (node->right)
            qpush(&q, node->right, hd + 1);
    }

    // Печатаем сверху-вниз: от минимального hd к максимальному
    for (int hd = min_hd; hd <= max_hd; ++hd) {
        int idx = hd + SHIFT;
        if (idx >= 0 && idx < MAX_HD && hd_map[idx].used)
            printf("%d ", hd_map[idx].key);
    }
    printf("\n");

    free(q.data);
}

// -----------------------------------------------------
// (опционально) Освобождение дерева
// -----------------------------------------------------
void free_tree(tree *root)
{
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

// -----------------------------------------------------
// main — чтение входных данных, построение дерева, вызов btUpView
// Вход: числа, заканчивающиеся 0 (0 не вставляем).
// -----------------------------------------------------
int main(void)
{
    tree *root = NULL;
    int x;

    while (scanf("%d", &x) == 1 && x != 0) {
        root = insert(root, x);
    }

    btUpView(root);

    free_tree(root);
    return 0;
}