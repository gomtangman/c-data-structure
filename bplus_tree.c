#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define t 3


struct Node
{
    int num_key;
    int key[2*t - 1];
    int leaf;
    void *child[2*t];
};


struct bp_tree
{
    struct Node *root;
};


void creat_bp_tree(struct bp_tree *bptr)
{
    struct Node *root_node = (struct Node*)malloc(sizeof(struct Node));
    root_node->leaf = 1;
    root_node->num_key = 0;
    bptr->root = root_node;
    printf("location of root is %p\n", bptr->root);

    return;
}


void split_bp_tree(struct Node *parent_node, int pos)
{
    struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
    struct Node *full_node = parent_node->child[pos];
    new_node->leaf = full_node->leaf;

    // full_node가 leaf node일 때
    if (full_node->leaf)
    {
        // new_node의 키 개수는 (t - 1) + index_key = t
        new_node->num_key = t;

        // new_node에 full_node의 키 나눠주기
        for (int i = 0; i < t; i++)
        {
            new_node->key[i] = full_node->key[i + t - 1];
        }

        // new_node에 full_node의 child 나눠주기
        for (int i = 0; i < t; i++)
        {
            new_node->child[i] = full_node->child[i + t - 1];
        }

        // full_node에서 사용되지 않는 마지막 child에 new_node 붙여주기
        full_node->child[2*t - 1] = new_node;

        // new_node가 마지막 노드가 아니라면 new_node의 마지막 child에 parent_node->child[pos + 1] 붙여주기
        if (parent_node->num_key > pos)
        {
            new_node->child[2*t - 1] = parent_node->child[pos + 1];
        }
    }

    // full_node가 internal node일 때
    else
    {
        // new_node의 키 개수는 t - 1
        new_node->num_key = t - 1;

        // new_node에 full_node의 키 나눠주기
        for (int i = 0; i < t - 1; i++)
        {
            new_node->key[i] = full_node->key[i + t];
        }

        // new_node에 full_node의 child 나눠주기
        for (int i = 0; i < t; i++)
        {
            new_node->child[i] = full_node->child[i + t];
        }
    }

    // full_node의 키 개수는 t - 1
    full_node->num_key = t - 1;

    // parent_node에 new_node 붙여주기
    for (int i = parent_node->num_key; i >= pos + 1; i--)
    {
        parent_node->child[i + 1] = parent_node->child[i];
    }
    parent_node->child[pos + 1] = new_node;

    // parent_node에 full_node의 키 올려주기
    for (int i = parent_node->num_key - 1; i >= pos; i--)
    {
        parent_node->key[i + 1] = parent_node->key[i];
    }
    parent_node->key[pos] = full_node->key[t - 1];
    parent_node->num_key++;

    return;
}


void insert_nonfull_bp_tree(struct Node *target_node, int target_key, int *target_value)
{
    int i = target_node->num_key - 1;

    // target_node가 leaf node일 때
    if (target_node->leaf)
    {
        // target_key가 들어갈 자리 만들어주기
        while (i >= 0 && target_key < target_node->key[i])
        {
            target_node->key[i + 1] = target_node->key[i];
            target_node->child[i + 1] = target_node->child[i];
            i--;
        }

        // target_key와 target_value 넣기
        target_node->key[i + 1] = target_key;
        target_node->child[i + 1] = (void *)target_value;
        target_node->num_key++;
    }

    // target_node가 internal node일 때
    else
    {
        // target_key가 들어갈 자식 노드 위치 찾기
        while (i >= 0 && target_key < target_node->key[i])
        {
            i--;
        }
        i++;

        // target_key가 들어갈 자식 노드가 가득 차 있다면
        if (((struct Node*)target_node->child[i])->num_key == 2*t - 1)
        {
            split_bp_tree(target_node, i);
            if (target_key > target_node->key[i])
            {
                i++;
            }
        }
        insert_nonfull_bp_tree(((struct Node*)target_node->child[i]), target_key, target_value);
    }
    
    return;
}


void insert_bp_tree(struct bp_tree *bptr, int target_key, int *target_value)
{
    struct Node *root_node = bptr->root;

    // root_node가 가득 찼다면
    if (root_node->num_key == 2*t - 1)
    {
        struct Node *new_root_node = (struct Node*)malloc(sizeof(struct Node));
        bptr->root = new_root_node;
        new_root_node->leaf = 0;
        new_root_node->num_key = 0;
        new_root_node->child[0] = root_node;

        split_bp_tree(new_root_node, 0);
        insert_nonfull_bp_tree(new_root_node, target_key, target_value);
    }

    // root가 가득 차지 않았다면
    else
    {
        insert_nonfull_bp_tree(root_node, target_key, target_value);
    }
    
    return;
}


struct Answer
{
    struct Node *res_node;
    int res_index;
};


struct Answer search_bp_tree(struct Node *root_node, int target_key)
{
    int i = 0;
    struct Answer ans;

    if (root_node->leaf)
    {
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
        else
        {
            ans.res_node = NULL;
            ans.res_index = -1;
            return ans;
        }
    }

    else
    {
        while (i < root_node->num_key && target_key >= root_node->key[i])
        {
            i++;
        }

        return search_bp_tree(root_node->child[i], target_key);
    }
}


void delete_bp_tree(struct bp_tree *bptr ,struct Node *target_node, int target_key)
{
    int i = 0;

    // target_node가 leaf node이면 target_key를 지운다
    if (target_node->leaf)
    {
        printf("delete %d from leaf node\n", target_key);
        // target_key가 있는 index 찾기
        while (i < target_node->num_key && target_key > target_node->key[i])
        {
            i++;
        }

        // target_key가 있는 index까지 key와 data 한 칸씩 당기기
        for (int j = i + 1; j < target_node->num_key; j++)
        {
            target_node->key[j - 1] = target_node->key[j];
            target_node->child[j - 1] = target_node->child[j];
        }
        target_node->num_key--;
        printf("delete case 1 done\n");
    }

    // target_node가 internal node이면
    else
    {
        // target_key가 속해있을 child 위치 찾기
        while (i < target_node->num_key && target_key >= target_node->key[i])
        {
            i++;
        }

        // 다음에 내려갈 child의 key 개수가 t - 1이면
        if (((struct Node*)target_node->child[i])->num_key == t - 1)
        {
            // 인접한 형제노드를 결정
            struct Node *adj_node;
            struct Node *child_node = target_node->child[i];
            int split_key;
            if (i == target_node->num_key || (i && ((struct Node*)target_node->child[i + 1])->num_key == t - 1))
            {
                printf("i and num_key is %d, %d and adj_node is i - 1\n", i, target_node->num_key);
                adj_node = target_node->child[i - 1];
                split_key = 1;
            }
            else
            {
                printf("i and num_key is %d, %d and adj_node is i + 1\n", i, target_node->num_key);
                adj_node = target_node->child[i + 1];
                split_key = 0;
            }

            // case 2.a 인접한 형제노드가 key를 t개 이상 갖고 있다면
            if (adj_node->num_key >= t)
            {
                if (split_key)
                {
                    if (child_node->leaf)
                    {
                        // adj_node의 극단에 있는 키를 separator에 넣는다
                        target_node->key[i - split_key] = adj_node->key[adj_node->num_key - 1];

                        // target_node에서 separator를 child_node로 키를 내려준다
                        for (int j = child_node->num_key - 1; j >= 0; j--)
                        {
                            child_node->key[j + 1] = child_node->key[j];
                        }
                        child_node->key[0] = target_node->key[i - split_key];

                        // adj_node의 child를 child_node로 옮겨준다
                        for (int j = 0; j < child_node->num_key; j++)
                        {
                            child_node->child[j + 1] = child_node->child[j];
                        }
                        child_node->child[0] = adj_node->child[adj_node->num_key - 1];
                    }
                    else
                    {
                        // target_node에서 separator를 child_node로 키를 내려준다
                        for (int j = child_node->num_key - 1; j >= 0; j--)
                        {
                            child_node->key[j + 1] = child_node->key[j];
                        }
                        child_node->key[0] = target_node->key[i - split_key];

                        // adj_node의 극단에 있는 키를 separator에 넣는다
                        target_node->key[i - split_key] = adj_node->key[adj_node->num_key - 1];
                        
                        // adj_node의 child를 child_node로 옮겨준다
                        for (int j = child_node->num_key; j >= 0; j--)
                        {
                            child_node->child[j + 1] = child_node->child[j];
                        }
                        child_node->child[0] = adj_node->child[adj_node->num_key];
                    }

                    // child_node와 adj_node의 num_key 변경
                    child_node->num_key++;
                    adj_node->num_key--;
                }
                else
                {
                    if (child_node->leaf)
                    {
                        // adj_node의 극단에 있는 키를 child_node로 옮겨준다
                        child_node->key[child_node->num_key] = adj_node->key[0];

                        // adj_node의 키를 당긴다
                        for (int j = 1; j < adj_node->num_key; j++)
                        {
                            adj_node->key[j - 1] = adj_node->key[j];
                        }

                        // adj_node의 극단에 있는 키를 separator에 넣는다
                        target_node->key[i - split_key] = adj_node->key[0];

                        // adj_node의 child를 child_node로 옮겨준다
                        child_node->child[child_node->num_key] = adj_node->child[0];
                        for (int j = 1; j < adj_node->num_key; j++)
                        {
                            adj_node->child[j - 1] = adj_node->child[j];
                        }
                    }

                    else
                    {
                        // target_node에서 separator를 child_node로 키를 내려준다
                        child_node->key[child_node->num_key] = target_node->key[i - split_key];

                        // adj_node의 극단에 있는 키를 separator에 넣는다
                        target_node->key[i - split_key] = adj_node->key[0];

                        // adj_node의 키를 당긴다
                        for (int j = 1; j < adj_node->num_key; j++)
                        {
                            adj_node->key[j - 1] = adj_node->key[j];
                        }

                        // adj_node의 child를 child_node로 옮겨준다
                        child_node->child[child_node->num_key + 1] = adj_node->child[0];
                        for (int j = 1; j <= adj_node->num_key; j++)
                        {
                            adj_node->child[j - 1] = adj_node->child[j];
                        }
                    }
                    
                    // child_node와 adj_node의 num_key 변경
                    child_node->num_key++;
                    adj_node->num_key--;
                }

                printf("case 2.a done\n");
            }

            // case 2.b 인접한 형제노드가 key를 t - 1개 갖고 있다면
            else
            {
                if (split_key)
                {
                    if(child_node->leaf)
                    {
                        // child_node의 키를 병합하면서 추가되는 key 개수만큼 옮긴다
                        // leaf 여부에 따라서 개수가 달라진다
                        for (int j = 0; j < child_node->num_key; j++)
                        {
                            child_node->key[j + t - 1] = child_node->key[j];
                        }

                        // leaf일 때는 separator를 병합하지 않는다
                    
                        // adj_node의 키를 child_node로 옮긴다
                        // leaf 여부에 따라서 위치가 달라진다
                        for (int j = 0; j < child_node->num_key; j++)
                        {
                            child_node->key[j] = adj_node->key[j];
                        }

                        // child_node에 adj_node의 child를 옮긴다
                        // leaf 여부에 따라서 개수가 달라진다
                        for (int j = 0; j < child_node->num_key; j++)
                        {
                            child_node->child[j + t - 1] = child_node->child[j];
                            child_node->child[j] = adj_node->child[j];
                        }

                        // child_node의 num_key를 바꾼다
                        child_node->num_key = 2*t - 2;

                        // leaf 노드의 연결리스트를 만든다
                        if (i - split_key)
                        {
                            ((struct Node*)target_node->child[i - 2])->child[2*t - 1] = child_node;
                        }
                    }
                    else
                    {
                        // child_node의 키를 병합하면서 추가되는 key 개수만큼 옮긴다
                        for (int j = 0; j < child_node->num_key; j++)
                        {
                            child_node->key[j + t] = child_node->key[j];
                        }
                        
                        // leaf가 아닐 때는 separator도 병합한다
                        child_node->key[t - 1] = target_node->key[i - split_key];

                        // adj_node의 키를 child_node로 옮긴다
                        for (int j = 0; j < child_node->num_key; j++)
                        {
                            child_node->key[j] = adj_node->key[j];
                        }

                        // child_node에 adj_node의 child를 옮긴다
                        for (int j = 0; j <= child_node->num_key; j++)
                        {
                            child_node->child[j + t] = child_node->child[j];
                            child_node->child[j] = adj_node->child[j];
                        }

                        // child_node의 num_key를 바꾼다
                        child_node->num_key = 2*t - 1;
                    }

                    // target_node에서 separator를 없앤다
                    for (int j = i - split_key + 1; j < target_node->num_key; j++)
                    {
                        target_node->key[j - 1] = target_node->key[j];
                    }

                    // target_node의 child를 당긴다
                    for (int j = i - split_key + 1; j <= target_node->num_key; j++)
                    {
                        target_node->child[j - 1] = target_node->child[j];
                    }

                    // 형제 노드를 없애고 key개수를 바꾼다
                    free(adj_node);
                    target_node->num_key--;
                    i--;
                }
                else
                {
                    if (child_node->leaf)
                    {
                        // adj_node의 키를 child_node로 옮긴다
                        // leaf 여부에 따라서 위치가 달라진다
                        for (int j = 0; j < child_node->num_key; j++)
                        {
                            child_node->key[j + t - 1] = adj_node->key[j];
                        }

                        // child_node에 adj_node의 child를 옮긴다
                        // leaf 여부에 따라서 개수가 달라진다
                        for (int j = 0; j <= child_node->num_key; j++)
                        {
                            child_node->child[j + t - 1] = adj_node->child[j];
                        }

                        // child_node의 num_key를 바꾼다
                        child_node->num_key = 2*t - 2;

                        // leaf 노드의 연결리스트를 만든다
                        if (i + 1 != target_node->num_key)
                        {
                            child_node->child[2*t - 1] = target_node->child[i + 2];
                        }
                    }
                    else
                    {
                        // leaf가 아닐 때는 separator도 병합한다
                        child_node->key[t - 1] = target_node->key[i - split_key];

                        // adj_node의 키를 child_node로 옮긴다
                        for (int j = 0; j < child_node->num_key; j++)
                        {
                            child_node->key[j + t] = adj_node->key[j];
                        }

                        // child_node에 adj_node의 child를 옮긴다
                        for (int j = 0; j <= child_node->num_key; j++)
                        {
                            child_node->child[j + t] = adj_node->child[j];   
                        }

                        // child_node의 num_key를 바꾼다
                        child_node->num_key = 2*t - 1;
                    }

                    // target_node에서 separator를 없앤다
                    for (int j = i - split_key + 1; j < target_node->num_key; j++)
                    {
                        target_node->key[j - 1] = target_node->key[j];
                    }

                    // target_node의 child를 당긴다
                    for (int j = i - split_key + 2; j <= target_node->num_key; j++)
                    {
                        target_node->child[j - 1] = target_node->child[j];
                    }

                    // 형제 노드를 없애고 key개수를 바꾼다
                    free(adj_node);
                    target_node->num_key--;
                }

                printf("case 2.b done\n");
            }
        }

        delete_bp_tree(bptr, target_node->child[i], target_key);
        printf("delete case 2 done\n");
    }

    if (!target_node->num_key)
    {
        bptr->root = target_node->child[0];
    }
}


void dfs(struct Node *target_node)
{
    struct Node *next_node;
    if (target_node->leaf)
    {
        for (int i = 0; i < target_node->num_key; i++)
        {
            printf("key %d is safe\n", target_node->key[i]);
        }
        // printf("connected list is pointing %d key\n", ((struct Node*)target_node->child[2*t - 1])->key[0]);
        return;
    }
    else
    {
        for (int i = 0; i <= target_node->num_key; i++)
        {
            next_node = (struct Node*)target_node->child[i];
            dfs(next_node);
        }
        return;
    }
}


int main()
{
    // 빈 비트리 생성
    struct bp_tree my_bptr;

    creat_bp_tree(&my_bptr);
    printf("function call is done, num_key is %d\n", my_bptr.root->num_key);

    // // 순서대로 넣기
    // for (int i = 1; i <= 36; i++)
    // {
    //     int temp_key = i;
    //     int temp_value = i*i + 3*i + 2;

    //     insert_bp_tree(&my_bptr, temp_key, &temp_value);
    //     printf("insert %d keys in bp_tree done and root has key[0] %d\n", i, my_bptr.root->key[0]);
    // }


    // 랜덤하게 넣기
    int key_arr[35] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230, 240, 250, 260, 9, 39, 101, 102, 103, 104, 161, 191, 251};
    for (int i = 0; i < 35; i++)
    {
        int temp_key = key_arr[i];
        int *temp_value = malloc(sizeof(int));
        *temp_value = temp_key;

        insert_bp_tree(&my_bptr, temp_key, temp_value);
        printf("insert %d keys in bp_tree done and root has key[0] %d\n", i + 1, my_bptr.root->key[0]);
    }

    // bp_tree 삭제
    printf("type key value to delete\n");
    int input_key[15] = {103, 70, 130, 104, 60, 120, 240, 160, 180, 250, 20, 80, 102, 50, 90};
    for (int i = 0; i < 15; i++)
    {
        int del_key = input_key[i];
        delete_bp_tree(&my_bptr, my_bptr.root, del_key);
    }
    
    // bp_tree 검색
    int count = 0;
    for (int i = 0; i < 35; i++)
    {
        struct Answer search_res = search_bp_tree(my_bptr.root, key_arr[i]);
        printf("key %d is in node %p index %d\n", key_arr[i], search_res.res_node, search_res.res_index);
        if (search_res.res_index == -1)
        {
            count++;
        }
        else
        {
            printf("value of key %d is %d\n", key_arr[i], *((int *)search_res.res_node->child[search_res.res_index]));
        }
        
    }

    printf("%d\n", count);




    // // 서브루틴 테스트용 트리 생성
    // struct Node temp_node1;
    // temp_node1.key[0] = 1;
    // temp_node1.key[1] = 2;
    // temp_node1.key[2] = 3;
    // int data1 = 10;
    // int data2 = 5;
    // int data3 = 0;
    // temp_node1.child[0] = &data1;
    // temp_node1.child[1] = &data2;
    // temp_node1.child[2] = &data3;
    // temp_node1.num_key = 3;
    // temp_node1.leaf = 1;
    // printf("temp_node1 set\n");

    // struct Node temp_node2;
    // temp_node2.key[0] = 7;
    // temp_node2.key[1] = 8;
    // temp_node2.key[2] = 9;
    // int data4 = 100;
    // int data5 = 50;
    // int data6 = 25;
    // temp_node2.child[0] = &data4;
    // temp_node2.child[1] = &data5;
    // temp_node2.child[2] = &data6;
    // temp_node2.num_key = 3;
    // temp_node2.leaf = 1;
    // printf("temp_node2 set\n");

    // my_bptr.root->leaf = 0;
    // my_bptr.root->key[0] = 5;
    // my_bptr.root->num_key = 1;
    // my_bptr.root->child[0] = &temp_node1;
    // my_bptr.root->child[1] = &temp_node2;
    // printf("root set\n");

    // // 노드 나누기 테스트
    // split_bp_tree(my_bptr.root, 0);
    // printf("split done with %d key node and %d key node\n", ((struct Node*)my_bptr.root->child[0])->num_key, ((struct Node*)my_bptr.root->child[1])->num_key);
    // printf("key of new_node is %d %d\n", ((struct Node*)my_bptr.root->child[1])->key[0], ((struct Node*)my_bptr.root->child[1])->key[1]);

    // // 덜 찬 노드에 넣기 테스트
    // insert_nonfull_bp_tree(my_bptr.root, 4, &data3);
    // printf("insert done and key inserted is %d", ((struct Node*)my_bptr.root->child[1])->key[2]);

    return 0;
}