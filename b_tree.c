#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define t 3


// t = 2짜리 b트리를 우선 만들자
struct Node
{
    int num_key;
    int key[2*t - 1];
    int value[2*t - 1];
    int leaf;
    struct Node *child[2*t];
};


struct b_tree
{
    struct Node *root;
};


void create_b_tree(struct b_tree *btr)
{
    struct Node *root_node = (struct Node*)malloc(sizeof(struct Node));
    root_node->leaf = 1;
    root_node->num_key = 0;
    btr->root = root_node;
    printf("position of root is %p\n", btr->root);
    printf("number of keys in root is %d\n", btr->root->num_key);

    return;
}


void split_b_tree(struct Node *parent_node, int pos)
{
    struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
    struct Node *full_node = parent_node->child[pos];
    new_node->leaf = full_node->leaf;
    new_node->num_key = t - 1;

    for (int i = 0; i < t - 1; i++)
    {
        new_node->key[i] = full_node->key[i + t];
        new_node->value[i] = full_node->value[i + t];
    }

    if (!full_node->leaf)
    {
        for (int i = 0; i < t; i++)
        {
            new_node->child[i] = full_node->child[i + t];
        }
    }

    full_node->num_key = t - 1;

    for (int i = parent_node->num_key; i >= pos + 1; i--)
    {
        parent_node->child[i + 1] = parent_node->child[i];
    }
    parent_node->child[pos + 1] = new_node;

    for (int i = parent_node->num_key - 1; i >= pos; i--)
    {
        parent_node->key[i + 1] = parent_node->key[i];
        parent_node->value[i + 1] = parent_node->value[i];
    }
    parent_node->key[pos] = full_node->key[t - 1];
    parent_node->value[pos] = full_node->value[t - 1];
    parent_node->num_key++;

    return;
}


void insert_nonfull_b_tree(struct Node *target_node, int target_key, int target_value)
{
    int i = target_node->num_key - 1;
    if (target_node->leaf)
    {
        while (i >= 0 && target_key < target_node->key[i])
        {
            target_node->key[i + 1] = target_node->key[i];
            target_node->value[i + 1] = target_node->value[i];
            i--;
        }
        target_node->key[i + 1] = target_key;
        target_node->value[i + 1] = target_value;
        target_node->num_key++;
    }
    else
    {
        while (i >= 0 && target_key < target_node->key[i])
        {
            i--;
        }
        i++;

        if (target_node->child[i]->num_key == 2*t - 1)
        {
            split_b_tree(target_node, i);
            if (target_key > target_node->key[i])
            {
                i++;
            }
        }
        insert_nonfull_b_tree(target_node->child[i], target_key, target_value);
    }
    
    return;
}


void insert_b_tree(struct b_tree *btr, int target_key, int target_value)
{
    struct Node *root_node = btr->root;
    if (root_node->num_key == 2*t - 1)
    {
        struct Node *new_root_node = (struct Node*)malloc(sizeof(struct Node));
        btr->root = new_root_node;
        new_root_node->leaf = 0;
        new_root_node->num_key = 0;
        new_root_node->child[0] = root_node;

        split_b_tree(new_root_node, 0);
        insert_nonfull_b_tree(new_root_node, target_key, target_value);
    }
    else
    {
        insert_nonfull_b_tree(root_node, target_key, target_value);
    }
    
    return;
}


struct Answer
{
    struct Node *res_node;
    int res_index;
};


struct Answer search_b_tree(struct Node *root_node, int target_key)
{
    int i = 0;
    struct Answer ans;
    while (i < root_node->num_key && target_key > root_node->key[i])
    {
        i++;
    }

    if (i < root_node->num_key && target_key == root_node->key[i])
    {
        ans.res_node = root_node;
        ans.res_index = i;

        return ans;
    }
    else if (root_node->leaf)
    {
        ans.res_node = NULL;
        ans.res_index = -1;
        return ans;
    }
    else
    {
        return search_b_tree(root_node->child[i], target_key);
    }
}


void delete_b_tree(struct b_tree *btr ,struct Node *target_node, int target_key)
{
    int i = 0;
    while (i < target_node->num_key && target_key > target_node->key[i])
    {
        i++;
    }

    if (i < target_node->num_key && target_key == target_node->key[i])
    {
        // case 1. target_key가 target_node에 있고 target_node->leaf == 1 이면 target_key를 target_node에서 삭제
        if (target_node->leaf)
        {
            printf("case 1 start\n");
            for (int j = i + 1; j < target_node->num_key; j++)
            {
                target_node->key[j - 1] = target_node->key[j];
                target_node->value[j - 1] = target_node->value[j];
            }

            target_node->num_key--;
            printf("delete case 1 done\n");
        }

        // case 2. target_key가 target_node에 있고 target_node->leaf == 0이면
        else
        {
            printf("case 2 start\n");
            // case a. target_key의 index에 해당하는 자식노드(y)가 키를 t개 이상 갖고 있다면 target_key의 predecessor를 재귀적으로 삭제하고 target_key를 predecessor로 변경
            if (target_node->child[i]->num_key >= t)
            {
                printf("case 2.a start\n");
                struct Node *pred_node = target_node->child[i];
                while (!pred_node->leaf)
                {
                    pred_node = pred_node->child[pred_node->num_key];
                }
                int pred_key = pred_node->key[pred_node->num_key - 1];
                int pred_value = pred_node->value[pred_node->num_key - 1];

                delete_b_tree(btr, pred_node, pred_key);
                target_node->key[i] = pred_key;
                target_node->value[i] = pred_value;

                printf("delete case 2.a done\n");
            }
            // case b. target_key의 index + 1에 해당하는 자식노드(z)가 키를 t개 이상 갖고 있다면 target_key의 successor를 재귀적으로 삭제하고 target_key를 successor로 변경
            else if (target_node->child[i + 1]->num_key >= t)
            {
                printf("case 2.b start\n");
                struct Node *succ_node = target_node->child[i + 1];
                while (!succ_node->leaf)
                {
                    succ_node = succ_node->child[0];
                }
                int succ_key = succ_node->key[0];
                int succ_value = succ_node->value[0];

                delete_b_tree(btr, succ_node, succ_key);
                target_node->key[i] = succ_key;
                target_node->value[i] = succ_value;

                printf("delete case 2.b done\n");
            }
            // case c. y에 target_key와 z의 키와 자식을 모두 넣고 z를 삭제한 후 재귀적으로 y에서 target_key를 삭제
            else
            {
                printf("case 2.c start\n");
                struct Node *pred_node = target_node->child[i];
                struct Node *succ_node = target_node->child[i + 1];

                // pred node에 다 넣자
                pred_node->key[t - 1] = target_node->key[i];
                pred_node->value[t - 1] = target_node->value[i];
                for (int j = 0; j < t - 1; j++)
                {
                    pred_node->key[t + j] = succ_node->key[j];
                    pred_node->value[t + j] = succ_node->value[j];
                }
                // pred_node의 t자식이 succ_node의 0자식
                if (!pred_node->leaf)
                {
                    for (int j = 0; j < t; j++)
                    {
                        pred_node->child[t + j] = succ_node->child[j];
                    }
                }

                // z와의 연결 지우기
                for (int j = i + 2; j <= target_node->num_key; j++)
                {
                    target_node->child[j - 1] = target_node->child[j];
                }
                free(succ_node);

                // target_node에서 target_key 지우기
                for (int j = i + 1; j < target_node->num_key; j++)
                {
                    target_node->key[j - 1] = target_node->key[j];
                    target_node->value[j - 1] = target_node->value[j];
                }
                target_node->num_key--;
                pred_node->num_key = 2*t - 1;

                delete_b_tree(btr, pred_node, target_key);

                printf("delete case 2.c done\n");
            }
        }
    }
    
    
    // case 3. target_key가 target_node에 없다면 target_key가 속할 범위에 해당하는 자식노드를 찾고
    else
    {
        printf("case 3 start\n");
        if (target_node->child[i]->num_key == t - 1)
        {
            struct Node *adj_node;
            struct Node *child_node = target_node->child[i];
            int split_key;
            if ( i == target_node->num_key || (i && target_node->child[i + 1]->num_key == t - 1))
            {
                printf("i and num_key is %d, %d and adj_node is i - 1\n", i, target_node->num_key);
                // i- split_key가 나눈다
                adj_node = target_node->child[i - 1];
                split_key = 1;
            }
            else
            {
                printf("i and num_key is %d, %d and adj_node is i + 1\n", i, target_node->num_key);
                // i - split_key가 나눈다
                adj_node = target_node->child[i + 1];
                split_key = 0;
            }

            // case a. 해당 노드가 키를 t-1개 갖고 있지만 인접한 형제가 키를 t개 이상 갖고 있다면 형제 노드와 해당 노드를 나누는 키를 해당 노드로 내려주고 형제 노드의 극단에 있는 키를 올리고 가장 끝 자식 포인터를 해당 노드로 옮긴다
            if (adj_node->num_key >= t)
            {
                if (split_key)
                {
                    for (int j = 0; j < child_node->num_key; j++)
                    {
                        child_node->key[j + 1] = child_node->key[j];
                        child_node->value[j + 1] = child_node->value[j];
                    }
                    child_node->key[0] = target_node->key[i - split_key];
                    child_node->value[0] = target_node->value[i - 1];

                    target_node->key[i - split_key] = adj_node->key[adj_node->num_key - 1];
                    for (int j = 0; j <= child_node->num_key; j++)
                    {
                        child_node->child[j + 1] = child_node->child[j];
                    }
                    child_node->child[0] = adj_node->child[adj_node->num_key];

                    child_node->num_key++;
                    adj_node->num_key--;
                }
                else
                {
                    child_node->key[child_node->num_key] = target_node->key[i - split_key];
                    child_node->value[child_node->num_key] = target_node->value[i - split_key];

                    target_node->key[i - split_key] = adj_node->key[0];

                    child_node->child[child_node->num_key + 1] = adj_node->child[0];
                    for (int j = 1; j < adj_node->num_key; j++)
                    {
                        adj_node->child[j - 1] = adj_node->child[j];
                    }

                    child_node->num_key++;
                    adj_node->num_key--;
                }
                
                printf("delete case 3.a done\n");
            }
            // case b. 해당 노드와 인접한 형제가 모두 키를 t-1개 갖고 있다면 형제 노드 하나, 해당 노드와 그 사이에 있는 키를 모두 병합
            else
            {
                if (split_key)
                {
                    for (int j = 0; j < child_node->num_key; j++)
                    {
                        child_node->key[j + t] = child_node->key[j];
                        child_node->value[j + t] = child_node->value[j];
                    }
                    child_node->key[t - 1] =  target_node->key[i - split_key];
                    child_node->value[t - 1] = target_node->value[i - split_key];
                    for (int j = i - split_key + 1; j < target_node->num_key; j++)
                    {
                        target_node->key[j - 1] = target_node->key[j];
                        target_node->value[j - 1] = target_node->value[j];
                    }
                    for (int j = 0; j < child_node->num_key; j++)
                    {
                        child_node->key[j] = adj_node->key[j];
                        child_node->value[j] = adj_node->value[j];
                    }

                    for (int j = 0; j <= child_node->num_key; j++)
                    {
                        child_node->child[j + t] = child_node->child[j];
                        child_node->child[j] = adj_node->child[j];
                    }

                    free(adj_node);
                    target_node->num_key--;
                    child_node->num_key = 2*t - 1;
                }
                else
                {
                    child_node->key[t - 1] =  target_node->key[i - split_key];
                    child_node->value[t - 1] = target_node->value[i - split_key];
                    for (int j = i - split_key + 1; j < target_node->num_key; j++)
                    {
                        target_node->key[j - 1] = target_node->key[j];
                        target_node->value[j - 1] = target_node->value[j];
                    }
                    for (int j = 0; j < child_node->num_key; j++)
                    {
                        child_node->key[j + t] = adj_node->key[j];
                        child_node->value[j + t] = adj_node->value[j];
                    }

                    for (int j = 0; j <= child_node->num_key; j++)
                    {
                        child_node->child[j + t] = adj_node->child[j];
                    }

                    free(adj_node);
                    target_node->num_key--;
                    child_node->num_key = 2*t - 1;
                }
                printf("delete case 3.b done\n");
            }
            
        }
        
        // 해당 자식노드에서 재귀적으로 target_key 삭제
        delete_b_tree(btr, target_node->child[i], target_key);

        printf("delete case 3 done\n");
    }
    
    if (!target_node->num_key)
    {
        btr->root = target_node->child[0];
    }
}


int main()
{
    // 빈 비트리 생성
    struct b_tree my_btr;

    create_b_tree(&my_btr);
    printf("function call is done, num_key is %d\n", my_btr.root->num_key);

    // // 서브루틴 테스트용 트리 생성
    // struct Node temp_node1;
    // temp_node1.key[0] = 1;
    // temp_node1.key[1] = 2;
    // temp_node1.key[2] = 3;
    // temp_node1.value[0] = 10;
    // temp_node1.value[1] = 5;
    // temp_node1.value[2] = 0;
    // temp_node1.num_key = 3;
    // temp_node1.leaf = 1;
    // printf("temp_node1 set\n");

    // struct Node temp_node2;
    // temp_node2.key[0] = 7;
    // temp_node2.key[1] = 8;
    // temp_node2.key[2] = 9;
    // temp_node2.value[0] = 100;
    // temp_node2.value[1] = 50;
    // temp_node2.value[2] = 25;
    // temp_node2.num_key = 3;
    // temp_node2.leaf = 1;
    // printf("temp_node2 set\n");

    // my_btr.root->leaf = 0;
    // my_btr.root->key[0] = 5;
    // my_btr.root->value[0] = 33;
    // my_btr.root->num_key = 1;
    // my_btr.root->child[0] = &temp_node1;
    // my_btr.root->child[1] = &temp_node2;
    // printf("root set\n");

    // // 노드 나누기 테스트
    // // split_b_tree(my_btr.root, 0);
    // // printf("split done with %d key node and %d key node\n", my_btr.root->child[0]->num_key, my_btr.root->child[1]->num_key);

    // // 덜 찬 노드에 넣기 테스트
    // insert_nonfull_b_tree(my_btr.root, 4, 77);
    // printf("insert done and key inserted is %d", my_btr.root->child[1]->key[1]);

    // for (int i = 1; i <= 20; i++)
    // {
    //     int temp_key = i;
    //     int temp_value = i*i + 3*i + 2;

    //     insert_b_tree(&my_btr, temp_key, temp_value);
    //     printf("insert i keys in b_tree done and root has key[0] %d\n", my_btr.root->key[0]);
    // }

    // // b_tree 검색
    // for (int i = 1; i <= 20; i++)
    // {
    //     struct Answer search_res = search_b_tree(my_btr.root, i);
    //     printf("key %d is in node %p index %d\n", i, search_res.res_node, search_res.res_index);
    // }




    int key_arr[36] = {10, 11, 12, 13, 14, 22, 23, 4, 5, 6, 15, 21, 16, 17, 1, 33, 34, 7, 30, 31, 32, 9, 18, 19, 35, 36, 24, 25, 29, 2, 3, 8, 20, 26, 27, 28};
    for (int i = 0; i < 36; i++)
    {
        int temp_key = key_arr[i];
        int temp_value = temp_key*temp_key + 3*temp_key + 2;

        insert_b_tree(&my_btr, temp_key, temp_value);
        printf("insert i keys in b_tree done and root has key[0] %d\n", my_btr.root->key[0]);
    }





    // b_tree 삭제
    printf("type key value to delete\n");
    int input_key;
    scanf("%d", &input_key);
    printf("%d\n", my_btr.root->key[0]);
    delete_b_tree(&my_btr, my_btr.root, input_key);

    return 0;
}