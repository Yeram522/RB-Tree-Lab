#include "rbtree.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
    
    rbtree* r = new_rbtree();
    rbtree_insert(r, 20);
    rbtree_insert(r, 10);
    rbtree_insert(r, 30);
    rbtree_insert(r, 40);
    rbtree_insert(r, 25);
    rbtree_insert(r, 7);
    rbtree_insert(r, 8);
    rbtree_insert(r, 11);
    rbtree_insert(r, 59);
    rbtree_insert(r, 90);
    rbtree_insert(r, 1);
    
    printf("%d\n",2);
    

    //find key
    node_t* find = rbtree_find(r,30);

    rbtree_erase(r, find);

    find = rbtree_find(r,100);
    
    //rbtree min
    find = rbtree_min(r);

    //rbtree max
    find = rbtree_max(r);

    //delete_rbtree(r);
    return 0;
}