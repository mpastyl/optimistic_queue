#include <stdio.h>
#include <stdlib.h>
#include "timers_lib.h"


struct node_t{
    int value;
    struct node_t * next;
    struct node_t * prev;
};


struct queue_t{
    struct node_t * Tail;
    struct node_t * Head;
};


void initialize(struct queue_t * Q){
    struct node_t * node = (struct node_t * ) malloc(sizeof(struct node_t));
    node->next =  NULL;
    node->prev =  NULL;
    node->value =  101;
    Q->Head  =  node;
    Q->Tail =  node;
}


void enqueue(struct queue_t * Q,int val){
    
    struct node_t * tail;
    struct node_t * nd = (struct node_t *) malloc(sizeof(struct node_t));
    nd->value = val;
    while (1){
        tail = Q->Tail;
        nd->next =  tail;
        if (__sync_bool_compare_and_swap(&Q->Tail,tail,nd)){
            tail->prev = nd;
            break;
        }
    }

}




int main(int argc,char * argv[]){


    struct queue_t * Q = (struct queue_t *) malloc(sizeof(struct queue_t));
    initialize(Q);

    enqueue(Q,5);
    enqueue(Q,8);
    enqueue(Q,12);
    printf(" %d \n",Q->Head->value);
    printf(" %d \n",Q->Head->prev->value);
    printf(" %d \n",Q->Head->prev->prev->value);
    return 1;
}
