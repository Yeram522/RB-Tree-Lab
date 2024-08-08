#include "rbtree.h"
#include <stdlib.h>

typedef struct stacknode{
  node_t *node;
  struct stacknode *next;
} stacknode;

node_t *left_rotate(rbtree*, node_t*);
node_t *right_rotate(rbtree*, node_t*);
node_t *rbtree_insert_fixup(rbtree *, node_t*);
node_t *rbtree_transplant(rbtree *, node_t*, node_t*);
node_t *rbtree_minimum(rbtree *, node_t*);
void rbtree_erase_fixup(rbtree*, node_t*);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  if(p == NULL)
    return NULL;

  // nil is the first node when new rbtree has created
  // 1. initialize Tnil node
  // because of waste of memory space
  // Tnil express all of the nodes nil in rbtree
  // Tnil = sentinel
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  // Tnil color is always black
  p->nil->color = RBTREE_BLACK; // #2 satisfy

  // 2. initialize rbtree
  p->root = p->nil;
  
  return p;
}

void delete_rbtree(rbtree *t) {
  if(t == NULL || t->root == NULL)
    return;

  // TODO: reclaim the tree nodes's memory
  if(t->root == t->nil){
    free(t->root);
    free(t);

    return; 
  }

  node_t *current = t->root;
  stacknode *stack = NULL;

  while (current != t->nil || stack != NULL)
  {
    if(current != t->nil){
      stacknode* newNode = (stacknode*)malloc(sizeof(stacknode));
      newNode->node = current; //value
      newNode->next = stack; //next stack

      stack = newNode;

      current = current->left; //move to left
    }
    else{
      //peek
      stacknode* peek = stack; //top of the stack
      stack = stack->next; // left
      current = peek->node;

      node_t *right = current->right;

      free(current);
      free(peek);

      current = right;
    }
  }
  
  free(t->nil);
  t->nil = NULL;
  free(t);
  t = NULL;

}

node_t *left_rotate(rbtree *t, node_t *x)
{
    node_t *y = x->right;

    x->right =  y->left;

    if(y->left != t->nil)
      y->left->parent = x;
    
    y->parent = x->parent;

    if(x->parent == t->nil)
      t->root = y;
    else if(x == x->parent->left){
      x->parent->left = y;
    }
    else x->parent->right = y;

    y->left = x;
    x->parent = y;

    // 내가 직접 짠거->작동되는데 책이 더 보기 좋아서 바꿈.., 위에는 책 수도코드
    /**cur->right->parent = cur->parent;
    
    if(cur == cur->parent->left){
      cur->parent->left = cur->right;
    }
    else{
      cur->parent->right = cur->right;
    }

    cur->parent = cur->right;
    cur->right = cur->right->left;
    cur->parent->left = cur;**/
    
    return t->root;
}

node_t *right_rotate(rbtree *t, node_t *x)
{
    node_t *y = x->left;

    x->left =  y->right; //y의 오른쪽 서브트리를 x의 왼쪽 서브트리로 옮긴다.

    if(y->right != t->nil)
      y->right->parent = x;
    
    y->parent = x->parent;

    if(x->parent == t->nil)
      t->root = y;
    else if(x == x->parent->left){ // x가 왼쪽 자식이라면?
      x->parent->left = y;
    }
    else x->parent->right = y;

    y->right = x;
    x->parent = y;

    // 내가 직접 짠거->작동되는데 책이 더 보기 좋아서 바꿈.., 위에는 책 수도코드

    /*if(cur == cur->parent->left){
      cur->parent->left = cur->left;
    }
    else{
      cur->parent->right = cur->left;
    }

    cur->parent = cur->left;
    cur->left = cur->left->right;
    cur->parent->right = cur;*/

    return t->root;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  if(t == NULL)
    return NULL;
  if(t->root == NULL)
    return NULL;

  node_t *new = (node_t*)calloc(1, sizeof(node_t));

  if(new == NULL)
    return NULL;

  new->color = RBTREE_RED; //new node is start with RED
  new->key = key;// initaialize key value

  //first insert
  if(t->root == t->nil)
  {
    t->root = new;

    new->parent = t->nil; //root node pointe T.nil
    new->left = t->nil;
    new->right = t->nil;
  
    new->color = RBTREE_BLACK;

    return  t->root;
  }
  
  // new: have to initialize parnet, left, right
  node_t *node = t->root; //compare target is key
  node_t *prevnode = t->nil; // key's parent will be

  while (node != t->nil)// until reach sentinel
  {
    prevnode = node;

    if(new->key < node->key) //descending to find own position
      node = node->left;
    else //if node in same or larger than key
      node = node->right;
  }

  //linking
  new->parent = prevnode;

  if(prevnode == t->nil) //is Empty
    t->root = new;
  else if(new->key < prevnode->key)
    prevnode->left = new;
  else
    prevnode->right = new;

  new->left = t->nil;
  new->right = t->nil;
  
  rbtree_insert_fixup(t,new);

  return t->root;
}

node_t *rbtree_insert_fixup(rbtree * t, node_t* cur)
{
  node_t* uncle = NULL; //initialize null 
  while (cur->parent->color == RBTREE_RED)
  {
    //is parent left child
    if(cur->parent == cur->parent->parent->left){
      uncle = cur->parent->parent->right; //new's uncle

      if(uncle->color == RBTREE_RED)// parent, uncle both RED?
      {
        cur->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;

        cur->parent->parent->color = RBTREE_RED;
        
        cur = cur->parent->parent; //change current node
      }
      else{// parent is RED and uncle is BLACK
        if(cur == cur->parent->right){// is right side?
          cur = cur->parent; //left rotate base on parent

          //left lotate : 
          left_rotate(t, cur);
        }
        cur->parent->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;

        //if(cur->parent->parent != NULL)
        right_rotate(t, cur->parent->parent);
      }
    }
    else{ //is parent right child
        uncle = cur->parent->parent->left;
        if(uncle->color == RBTREE_RED){
          cur->parent->color = RBTREE_BLACK;
          uncle->color = RBTREE_BLACK;

          cur->parent->parent->color = RBTREE_RED;
        
          cur = cur->parent->parent; //change current node
        }
        else{
          if(cur == cur->parent->left){// is right side?
            cur = cur->parent; //left rotate base on parent
            //left lotate : 
            right_rotate(t, cur);
          }
          cur->parent->color = RBTREE_BLACK;
          cur->parent->parent->color = RBTREE_RED;

          left_rotate(t, cur->parent->parent);
        }
    }
  }

  t->root->color = RBTREE_BLACK;
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  if(t == NULL)
    return NULL;
  if(t->root == NULL)
    return NULL;
  //start with root node
  node_t *current = t->root;

  while (current != t->nil)
  {
    if(current->key == key){
      return current;
    }

    if(current->key < key)
    {
      current = current->right;
    }
    else{
      current = current->left;
    }
  }
  

  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find

  //start with root node
  node_t *current = t->root;

  while (current->left != t->nil) //go most left side of tree
  {
    current = current->left;
  }
  

  return current;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find

  //start with root node
  node_t *current = t->root;

  while (current->right != t->nil)//go most right side of tree
  {
    current = current->right;
  }
  

  return current;
}

int rbtree_erase(rbtree *t, node_t *z) {
  if(t == NULL || z == NULL)
    return 0;
  if(t->root == NULL)
    return 0;
    

  node_t* x = NULL;
  node_t* y = z;
  color_t y_original_color = y->color;

  if(z->left == t->nil){
    x = z->right; //point y's child or nil node when have no childs 
    rbtree_transplant(t,z,z->right);
  }
  else if(z->right == t->nil)
  {
    x = z->left;
    rbtree_transplant(t,z,z->left);
  }
  else{
    y = rbtree_minimum(t, z->right);
    y_original_color = y->color;
    x = y->right;
    if( y != z->right)
    {
      rbtree_transplant(t,y,y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    else x->parent = y;

    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  if(y_original_color == RBTREE_BLACK)
    rbtree_erase_fixup(t,x);

  // 메모리 반환.
  free(z);
  z = NULL;
  return 0;
}

node_t *rbtree_transplant(rbtree *t, node_t *u, node_t *v){
  if( u->parent == t->nil)
    t->root = v;
  else if( u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
}

node_t *rbtree_minimum(rbtree *t, node_t *y)
{
    while (y->left != t->nil)
    {
      y = y->left;
    }
    return y;
}

void rbtree_erase_fixup(rbtree *t, node_t *x)
{
  node_t* w = NULL; //uncle

  //x 가 root가 아니고, x.color 이 black일때만 루프 실행
  while(x != t->root && x->color == RBTREE_BLACK){
    if( x == x->parent->left){ //x의 부모가 왼쪽 노드일 때
        w = x->parent->right; //형제

        //case 1: 형제가 RED 인 경우
        if(w->color == RBTREE_RED){
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          left_rotate(t,x->parent);
          w = x->parent->right; //회전 했으므로 형제 갱신
        }
        //case 2 : 형제의 자식 둘다 BLACK일 떄
        if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
        {
          w->color = RBTREE_RED;
          x = x->parent;
        }
        else{
          if(w->right->color == RBTREE_BLACK){//case 3: 오른쪽 자식만 BLACK 일 때
            w->left->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            right_rotate(t, w);
            w = x->parent->right;
          }

          //case 4 : w는 BLACK이고 w의 오른쪽 자식은 RED 인 경우
          w->color = x->parent->color;
          x->parent->color = RBTREE_BLACK;
          w->right->color = RBTREE_BLACK;
          left_rotate(t, x->parent);
          x = t->root;
        }
    }
    else{ // x의 부모가 오른쪽 노드일 때
      w = x->parent->left; //형제

      //case 1: 형제가 RED 인 경우
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t,x->parent);
        w = x->parent->left; //회전 했으므로 형제 갱신
      }
      //case 2 : 형제의 자식 둘다 BLACK일 떄
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if(w->left->color == RBTREE_BLACK){//case 3: 오른쪽 자식만 BLACK 일 때
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
      }

      //case 4 : w는 BLACK이고 w의 오른쪽 자식은 RED 인 경우
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->left->color = RBTREE_BLACK;
      right_rotate(t, x->parent);
      x = t->root;
      }
    }
  }
  
  
  //x의 color을 black으로
  x->color = RBTREE_BLACK;
  
  return;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
    if (t == NULL || arr == NULL || t->root == t->nil)
        return 0;

    node_t* cur = t->root;
    int count = 0;

    while (cur != t->nil && count < n)
    {
        if (cur->left == t->nil)
        {
            // 왼쪽 자식이 없으면 현재 노드 처리
            arr[count++] = cur->key;
            cur = cur->right;
        }
        else
        {
            // 왼쪽 서브트리에서 현재 노드의 중위 선행자를 찾음
            node_t* pre = cur->left;
            while (pre->right != t->nil && pre->right != cur)
                pre = pre->right;

            if (pre->right == t->nil)
            {
                // 중위 선행자를 현재 노드에 연결
                pre->right = cur;
                cur = cur->left;
            }
            else
            {
                // 이미 연결되어 있다면 현재 노드 처리 후 연결 해제
                pre->right = t->nil;
                arr[count++] = cur->key;
                cur = cur->right;
            }
        }
    }

    return count;
}
