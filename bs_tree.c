// 오류 발생
// The terminal process "C:\WINDOWS\System32\cmd.exe /d /c cmd /C C:\Users\Ben\Desktop\SWjungle\jungle_week5\bs_tree" failed to launch (exit code: 3221225477).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Node
{
    struct Node *left;
    struct Node *right;
    struct Node *parent;
    int value;
};


struct BSTree
{
    struct Node *head;
};


struct Node *bst_search(struct Node *head, int key)
{
    if (head == NULL || head->value == key)
    {
        return head;
    }

    if (head->value > key)
    {
        return bst_search(head->left, key);
    }
    else
    {
        return bst_search(head->right, key);
    }
}


struct Node *bst_min(struct Node *head)
{
    while (head->left != NULL)
    {
        head = head->left;
    }

    return head;
}


struct Node *bst_max(struct Node *head)
{
    while (head->right != NULL)
    {
        head = head->right;
    }

    return head;
}


struct Node *bst_successor(struct Node *target)
{
    if (target->right != NULL)
    {
        target = target->right;
        return bst_min(target);
    }

    struct Node *parent_node;
    parent_node = target->parent;
    while (parent_node != NULL && target == parent_node->right)
    {
        target = parent_node;
        parent_node = target->parent;
    }

    return parent_node;
}


struct Node *bst_predecessor(struct Node *target)
{
    if (target->left != NULL)
    {
        target = target->left;
        return bst_max(target);
    }

    struct Node *parent_node;
    parent_node = target->parent;
    while (parent_node != NULL && target == parent_node->left)
    {
        target = parent_node;
        parent_node = target->parent;
    }

    return parent_node;
}


void bst_insert(struct BSTree *bst, struct Node *target)
{
    struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
    memcpy(new_node, target, sizeof(struct Node));

    if (bst->head == NULL)
    {    
        bst->head = new_node;
        return;
    }

    struct Node *cur_node;
    struct Node *parent_node;
    
    cur_node = bst->head;
    parent_node = NULL;
    while (cur_node != NULL)
    {
        if (cur_node->value > target->value)
        {
            parent_node = cur_node;
            cur_node = cur_node->left;
        }
        else
        {
            parent_node = cur_node;
            cur_node = cur_node->right;
        }
    }

    if (parent_node->value > target->value)
    {
        parent_node->left = new_node;
    }
    else
    {
        parent_node->right = new_node;
    }
    new_node->parent = parent_node;
    return;
}


void bst_transplant(struct BSTree *bst, struct Node *stuck_node, struct Node *rolling_node)
{
    if (stuck_node->parent == NULL)
    {
        bst->head = rolling_node;
    }
    else if (stuck_node == stuck_node->parent->left)
    {
        stuck_node->parent->left = rolling_node;
    }
    else
    {
        stuck_node->parent->right = rolling_node;
    }
    
    if (rolling_node != NULL)
    {
        rolling_node->parent = stuck_node->parent;
    }

    return;
}


void bst_delete(struct BSTree *bst, struct Node *target_node)
{
    if (target_node->left == NULL)
    {
        bst_transplant(bst, target_node, target_node->right);
    }
    else if (target_node->right == NULL)
    {
        bst_transplant(bst, target_node, target_node->left);
    }
    else
    {
        struct Node *suc_node = bst_min(target_node->right);
        if (suc_node->parent != target_node)
        {
            bst_transplant(bst, suc_node, suc_node->right);
            suc_node->right = target_node->right;
            suc_node->right->parent = suc_node;
        }

        bst_transplant(bst, target_node, suc_node);
        suc_node->left = target_node->left;
        suc_node->left->parent = suc_node;
    }

    free(target_node);
    return;
}


int main()
{
    struct BSTree *bst = (struct BSTree*)malloc(sizeof(struct BSTree));
    bst->head = NULL;

    for (int i = 1; i <= 10; i++)
    {
        struct Node temp;
        temp.value = i;
        temp.left = NULL;
        temp.right = NULL;
        temp.parent = NULL;
        bst_insert(bst, &temp);
    }

    struct Node *target;
    for (target = bst->head; target != NULL; target = bst->head)
    {
        bst_delete(bst, target);
        printf("%p\n", bst->head);
    }
    return 0;
}