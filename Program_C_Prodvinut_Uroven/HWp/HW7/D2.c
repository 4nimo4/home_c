/*
//------------------------------------------------------
//  D2-Найти брата
//------------------------------------------------------ 
В программе описано двоичное дерево:
typedef struct tree {
datatype key;
struct tree *left, *right;
struct tree *parent; //ссылка на родителя
} tree;
Требуется реализовать функцию, которая по ключу возвращает адрес соседнего элемента - брата. 
Если такого ключа нет или у узла нет брата, то необходимо вернуть 0.
Прототип функции:

tree * findBrother(tree *root, int key)
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct tree {
    int key;
    struct tree *left, *right;
    struct tree *parent;      // ссылка на родителя
} tree;

// Вставка в бинарное дерево поиска с заполнением parent
tree* insert(tree *root, int key, tree *parent)
{
    if (!root) {
        root = (tree*)calloc(1, sizeof(tree));
        if (!root) {
            fprintf(stderr, "Memory allocation error\n");
            exit(EXIT_FAILURE);
        }
        root->key    = key;
        root->left   = NULL;
        root->right  = NULL;
        root->parent = parent;
        return root;
    }

    if (key < root->key)
        root->left  = insert(root->left,  key, root);
    else if (key > root->key)
        root->right = insert(root->right, key, root);
    // при равенстве можно ничего не вставлять или всегда вправо — в примере дубликатов нет

    return root;
}

// Поиск узла по ключу (BST)
tree* search(tree *root, int key)
{
    if (!root || root->key == key)
        return root;
    if (key < root->key)
        return search(root->left, key);
    else
        return search(root->right, key);
}

// ------------------------------------------------------
// Поиск "брата" по ключу
// Если ключа нет или у найденного узла нет брата — вернуть 0 (NULL)
// ------------------------------------------------------
tree * findBrother(tree *root, int key)
{
    // 1. Находим узел по ключу
    tree *node = search(root, key);
    if (!node)
        return NULL;           // такого ключа нет

    // 2. Если нет родителя — это корень, брата нет
    if (!node->parent)
        return NULL;

    tree *parent = node->parent;

    // 3. Определяем, левый это сын или правый, и возвращаем второго
    if (parent->left == node)
        return parent->right;  // брат — правый сын (может быть NULL)
    else
        return parent->left;   // брат — левый сын (может быть NULL)
}

// (опционально) освобождение дерева
void free_tree(tree *root)
{
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main(void)
{
    tree *root = NULL;
    int x;

    // Вводим числа до 0 — это ключи для дерева
    while (scanf("%d", &x) == 1 && x != 0) 
    {
        root = insert(root, x, NULL);
    }

    // После 0 читаем ключ, для которого ищем "брата"
    int key_to_find;
    if (scanf("%d", &key_to_find) != 1) 
    {
        fprintf(stderr, "No brother key provided\n");
        free_tree(root);
        return 1;
    }

    tree *bro = findBrother(root, key_to_find);
    if (bro)
        printf("%d\n", bro->key);
    else
        printf("0\n");  // по условию можно вернуть 0 (здесь печатаем 0)

    free_tree(root);
    return 0;
}