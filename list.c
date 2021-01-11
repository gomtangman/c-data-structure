#include <stdio.h>
#include <stdlib.h>


struct Node
{
    struct Node *next;
    int value;
};


void list_add(struct Node *head, struct Node *target_node)
{
    // 리스트의 머리에 노드를 추가하는 함수
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    *new_node = *target_node;

    if (head->next == NULL)
    {
        new_node->next = NULL;
        head->next = new_node;
    }
    else
    {
        new_node->next = head->next;
        head->next = new_node;
    }
}


void list_remove(struct Node *head, int target_value)
{
    // 리스트에서 특정 값을 갖는 노드를 삭제하는 함수
    struct Node *target_node = head->next;
    struct Node *parent_node = head;

    while (target_node != NULL)
    {
        if (target_node->value == target_value)
        {
            parent_node->next = target_node->next;
            free(target_node);
            return;
        }
        parent_node = target_node;
        target_node = target_node->next;
    }

    printf("No match");
    return;
}


struct Node *list_search(struct Node *head, int target_value)
{
    // 리스트에서 특정 값을 찾는 함수
    struct Node *cur_node = head->next;

    while (cur_node != NULL)
    {
        if (cur_node->value == target_value)
        {
            return cur_node;
        }
        cur_node = cur_node->next;
    }
    return cur_node;
}


int main()
{
    struct Node *head = (struct Node*)malloc(sizeof(struct Node));
    head->next = NULL;

    for (int i = 1; i <= 10; i++)
    {
        struct Node temp;
        temp.value = i;
        list_add(head, &temp);
        printf("%d\n", head->next->value);
    }

    struct Node *target = (struct Node*)malloc(sizeof(struct Node));
    for (target = head->next; target != NULL; target = head->next)
    {
        list_remove(head, target->value);
        printf("%d\n", head->next->value);
    }

    return 0;
}