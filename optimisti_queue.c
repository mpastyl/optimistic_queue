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
        //unsigned long long temp =  Q->Tail.both;
        if (__sync_bool_compare_and_swap(&(Q->Tail.both),tail.both,new_to_set)){
            struct pointer_t prev =  ((struct node_t *)get_pointer(tail.both))->prev;
            prev.both = set_both(prev.both,nd,get_count(tail.both));
            ((struct node_t * ) get_pointer(tail.both))->prev.both = prev.both;
            break;
        }
    }

}

int dequeue(struct queue_t * Q,int * p_val){
    
    struct pointer_t  tail;
    struct pointer_t  head;
    struct pointer_t  firstNodePrev;
    int val;
    struct node_t * new_to_set;
    while (1){

        head = Q->Head;
        tail = Q->Tail;
        firstNodePrev = ((struct node_t *)get_pointer(head.both))->prev;
        val =  ((struct node_t * )get_pointer(head.both))-> value;
        if (head.both == Q->Head.both){
            if (val!=DUMMY_VAL){
                if (tail.both!=head.both){
                    //TODO: add tag check and fix list
                }

                else{
                    struct node_t * nd_dummy = (struct node_t * ) malloc(sizeof(struct node_t));
                    nd_dummy->value =  DUMMY_VAL;
                    nd_dummy->next.both = tail.both;
                    new_to_set = set_both(new_to_set,nd_dummy,get_count(tail.both)+1);
                    if (__sync_bool_compare_and_swap(&(Q->Tail.both),tail.both,new_to_set)){
                        ((struct node_t *)get_pointer(head.both))->prev.both =set_both(((struct node_t *)get_pointer(head.both))->prev.both,nd_dummy,get_count(tail.both));
                    }
                    else free(nd_dummy);
                    continue;
                }
                new_to_set =  set_both(new_to_set,firstNodePrev.both,get_count(head.both)+1);
                if(__sync_bool_compare_and_swap(&(Q->Head.both),head.both,new_to_set)){
                    free((struct node_t *)get_pointer(head.both));
                    *p_val = val;
                    return 1;
                }
            }
            else{
                if (tail.both == head.both) return 0;
                else {
                    //TODO: add tag check and call fixlist
                    new_to_set = set_both(new_to_set,firstNodePrev.both,get_count(head.both)+1);
                    int temp = __sync_bool_compare_and_swap(&(Q->Head.both),head.both,new_to_set);
                }
            }
        }
    }


            
}
/*
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
    res=dequeue(Q,&val);
    if (res) printf("  dequeued %d \n",val);
    enqueue(Q,8);
    res=dequeue(Q,&val);
    if (res) printf("  dequeued %d \n",val);
    enqueue(Q,22);
    res=dequeue(Q,&val);
    if (res) printf("  dequeued %d \n",val);
    //res=dequeue(Q,&val);
    //if (res) printf("  dequeued %d \n",val);
    enqueue(Q,12);
    enqueue(Q,7);
    enqueue(Q,18);
    printf(" %d \n",((struct node_t *)get_pointer(Q->Head.both))->value);
    struct pointer_t prev =((struct node_t *)get_pointer(Q->Head.both))->prev;
    printf(" %d \n",((struct node_t *)get_pointer(prev.both))->value);
    struct pointer_t prev_2 =((struct node_t *)get_pointer(prev.both))->prev;
    printf(" %d \n",((struct node_t *)get_pointer(prev_2.both))->value);
    //printf(" %d \n",Q->Head->prev->value);
    //printf(" %d \n",Q->Head->prev->prev->value);
    //printqueue(Q);
    return 1;
}
