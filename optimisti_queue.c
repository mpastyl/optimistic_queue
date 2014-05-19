#include <stdio.h>
#include <stdlib.h>
#include "timers_lib.h"


struct pointer_t{
    unsigned long long both;
};

struct node_t{
    int value;
    struct pointer_t  next;
    struct pointer_t  prev;
};


struct queue_t{
    struct pointer_t Tail;
    struct pointer_t Head;
};


int DUMMY_VAL = 101;

unsigned long long get_count(unsigned long long a){

    unsigned long long b = a >>48;
    return b;
}

unsigned long long get_pointer(unsigned long long a){
    unsigned long long b = a << 16;
    b= b >>16;
    return b;
}

unsigned long long set_count(unsigned long long  a, unsigned long long count){
    unsigned long long count_temp =  count << 48;
    unsigned long long b = get_pointer(a);
    b = b | count_temp;
    return b;
}

unsigned long long set_pointer(unsigned long long a, unsigned long long ptr){
    unsigned long long b = 0;
    unsigned long long c = get_count(a);
    b = set_count(b,c);
    ptr = get_pointer(ptr);
    b= b | ptr;
    return b;
}

unsigned long long set_both(unsigned long long a, unsigned long long ptr, unsigned long long count){
    a=set_pointer(a,ptr);
    a=set_count(a,count);
    return a;
}


void initialize(struct queue_t * Q){
    struct node_t * node = (struct node_t * ) malloc(sizeof(struct node_t));
    //node->next =  NULL;
    //node->prev =  NULL;
    node->value =  DUMMY_VAL;
    Q->Head.both  =  set_both(Q->Head.both,node,0);
    Q->Tail.both =  set_both(Q->Tail.both,node,0);
}


void enqueue(struct queue_t * Q,int val){
    
    struct pointer_t  tail;
    struct node_t * nd = (struct node_t *) malloc(sizeof(struct node_t));
    nd->value = val;
    while (1){
        tail = Q->Tail;
        nd->next.both = set_both(nd->next.both,tail.both,get_count(tail.both)+1);
        struct node_t * new_to_set = set_both(new_to_set,nd,get_count(tail.both)+1);
        unsigned long long temp =  Q->Tail.both;
        if (__sync_bool_compare_and_swap(&temp,tail.both,new_to_set)){
            struct pointer_t prev =  ((struct node_t *)get_pointer(tail.both))->prev;
            prev.both = set_both(prev.both,nd,get_count(tail.both));
            break;
        }
    }

}
/*
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

*/
int main(int argc,char * argv[]){

    int res,val=0;
    struct queue_t * Q = (struct queue_t *) malloc(sizeof(struct queue_t));
    initialize(Q);

    enqueue(Q,5);
    enqueue(Q,8);
    enqueue(Q,12);
    //res=dequeue(Q,&val);
    //res=dequeue(Q,&val);
    //if (res) printf("  dequeued %d \n",val);
    enqueue(Q,7);
    printf(" %d \n",((struct node_t *)get_pointer(Q->Head.both))->value);
    struct pointer_t prev =((struct node_t *)get_pointer(Q->Head.both))->prev;
    printf(" %d \n",((struct node_t *)get_pointer(prev.both))->value);
    //printf(" %d \n",Q->Head->prev->value);
    //printf(" %d \n",Q->Head->prev->prev->value);
    //printqueue(Q);
    return 1;
}
