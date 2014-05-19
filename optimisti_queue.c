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


int DUMMY_VAL = 101;

void initialize(struct queue_t * Q){
    struct node_t * node = (struct node_t * ) malloc(sizeof(struct node_t));
    node->next =  NULL;
    node->prev =  NULL;
    node->value =  DUMMY_VAL;
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

int dequeue(struct queue_t * Q,int * p_val){
    
    struct node_t * tail;
    struct node_t * head;
    struct node_t * firstNodePrev;
    int val;
    while (1){

        head = Q->Head;
        tail = Q->Tail;
        firstNodePrev = head->prev;
        val =  head-> value;
        if (head == Q->Head){
            if (val!=DUMMY_VAL){
                if (tail!=head){
                    //TODO: add tag check and fix list
                }

                else{
                    struct node_t * nd_dummy = (struct node_t * ) malloc(sizeof(struct node_t));
                    nd_dummy->value =  DUMMY_VAL;
                    nd_dummy->next = tail;
                    if (__sync_bool_compare_and_swap(&Q->Tail,tail,nd_dummy)){
                        head->prev = nd_dummy;
                    }
                    else free(nd_dummy);
                    continue;
                }
                if(__sync_bool_compare_and_swap(&Q->Head,head,firstNodePrev)){
                    free(head);
                    *p_val = val;
                    return 1;
                }
            }
            else{
                if (tail == head) return 0;
                else {
                    //TODO: add tag check and call fixlist
                    int temp = __sync_bool_compare_and_swap(&Q->Head,head,firstNodePrev);
                }
            }
        }
    }


            
}

void printqueue(struct queue_t * Q){

    struct node_t * curr ;
    struct node_t * next ;
    struct node_t * prev;
    curr = Q->Head;
    next = Q->Head->next;
    prev = Q->Head->prev;
    while (curr != Q->Tail){
        printf("%d ",curr->value);
        curr = prev;
        prev = curr ->prev;
    }
    printf("%d ",curr->value);
    printf("\n");

}


int main(int argc,char * argv[]){

    int res,val=0;
    struct queue_t * Q = (struct queue_t *) malloc(sizeof(struct queue_t));
    initialize(Q);

    enqueue(Q,5);
    enqueue(Q,8);
    enqueue(Q,12);
    res=dequeue(Q,&val);
    res=dequeue(Q,&val);
    //if (res) printf("  dequeued %d \n",val);
    enqueue(Q,7);
    //printf(" %d \n",Q->Head->value);
    //printf(" %d \n",Q->Head->prev->value);
    //printf(" %d \n",Q->Head->prev->prev->value);
    printqueue(Q);
    return 1;
}
