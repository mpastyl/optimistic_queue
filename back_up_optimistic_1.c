#include <stdio.h>
#include <stdlib.h>
#include "timers_lib.h"


struct pointer_t{
    __int128 both;
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


int DUMMY_VAL = 101;//TODO: change this

__int128 get_count(__int128 a){

    __int128 b = a >>64;
    return b;
}

struct node_t * get_pointer(__int128 a){
    __int128 b = a << 64;
    b= b >>64;
    return ((struct node_t *) b);
}
//get the counter out of a 128 bit integer
__int128 set_count(__int128  a, __int128 count){
    __int128 count_temp =  count << 64;
    __int128 b = get_pointer(a);
    b = b | count_temp;
    return b;
}
//get the pointer out of a 128bit integer
__int128 set_pointer(__int128 a, __int128 ptr){
    __int128 b = 0;
    __int128 c = get_count(a);
    b = set_count(b,c);
    ptr = get_pointer(ptr);
    b= b | ptr;
    return b;
}
// sets a's pointer to ptr and a's counter to count
__int128 set_both(__int128 a, __int128 ptr, __int128 count){
    a=set_pointer(a,(__int128 )ptr);
    a=set_count(a,count);
    return a;
}


void initialize(struct queue_t * Q){
    struct node_t * node = (struct node_t * ) malloc(sizeof(struct node_t));
    //node->next =  NULL;
    //node->prev =  NULL;
    node->value =  DUMMY_VAL;
    Q->Head.both  =  set_both(Q->Head.both,(__int128) node,0);
    Q->Tail.both =  set_both(Q->Tail.both,(__int128) node,0);
}


void enqueue(struct queue_t * Q,int val){
    
    struct pointer_t  tail;
    struct node_t * nd = (struct node_t *) malloc(sizeof(struct node_t));
    nd->value = val;
    while (1){
        tail = Q->Tail;
        //set next of new node
        nd->next.both = set_both(nd->next.both, tail.both, get_count(tail.both)+(                                                                      __int128)1);
        //new to set is the new value to set via CAS
        __int128 new_to_set = set_both(new_to_set,(__int128)nd, get_count(tail.both)+(__int128)1);
        if(__sync_bool_compare_and_swap(&(Q->Tail.both), tail.both, new_to_set)){
            // if cas is succesful we update prev without atomic
            //struct pointer_t prev =  get_pointer(tail.both)->prev;
            //prev.both = set_both(prev.both,(__int128)nd, get_count(tail.both));
            (get_pointer(tail.both))->prev.both = set_both(get_pointer(tail.both)->prev, (__int128)nd, get_count(tail.both));
            break;
        }
    }

}

void fixList(struct queue_t * Q,struct pointer_t tail,struct pointer_t head){
    
    struct pointer_t currNode;
    struct pointer_t currNodeNext;
    struct pointer_t nextNodePrev;

    currNode =  tail;
    while((head.both == Q->Head.both)&&(currNode.both != head.both)){
        currNodeNext = get_pointer(currNode.both)->next;
        if( get_count(currNodeNext.both)!=get_count(currNode.both)){
            return;
        }
        nextNodePrev = (get_pointer(currNodeNext.both))->prev;
        if((get_pointer(nextNodePrev.both)!=get_pointer(currNode.both))||(get_count(nextNodePrev.both)!=(get_count(currNode.both) -(__int128)1))){
            (get_pointer(currNodeNext.both))->prev.both = set_both((get_pointer(currNodeNext.both))->prev.both,currNode.both,get_count(currNode.both)-(__int128)1);
        }
        currNode.both = set_both(currNode.both,currNodeNext.both,get_count(currNode.both) -(__int128)1);
    }
}

int dequeue(struct queue_t * Q,int * p_val){
    
    struct pointer_t  tail;
    struct pointer_t  head;
    struct pointer_t  firstNodePrev;
    int val;
    __int128 new_to_set;
    while (1){

        head = Q->Head;
        tail = Q->Tail;
        firstNodePrev = (get_pointer(head.both))->prev;
        val =  (get_pointer(head.both))-> value;
        if (head.both == Q->Head.both){
            if (val!=DUMMY_VAL){
                if (tail.both!=head.both){
                    if (get_count(firstNodePrev.both)!=get_count(head.both)){
                        fixList(Q,tail,head);
                        continue;
                    }
                }

                else{
                    struct node_t * nd_dummy = (struct node_t * ) malloc(sizeof(struct node_t));
                    nd_dummy->value =  DUMMY_VAL;
                    nd_dummy->next.both = set_both(nd_dummy->next.both,tail.both,get_count(tail.both)+(__int128)1);
                    new_to_set = set_both(new_to_set,(__int128)nd_dummy,get_count(tail.both)+(__int128)1);
                    if (__sync_bool_compare_and_swap(&(Q->Tail.both),tail.both,new_to_set)){
                        (get_pointer(head.both))->prev.both =set_both((get_pointer(head.both))->prev.both,(__int128)nd_dummy,get_count(tail.both));
                    }
                    else free(nd_dummy);
                    continue;
                }
                new_to_set =  set_both(new_to_set,firstNodePrev.both,get_count(head.both)+(__int128)1);
                if(__sync_bool_compare_and_swap(&(Q->Head.both),head.both,new_to_set)){
                    free(get_pointer(head.both));
                    *p_val = val;
                    return 1;
                }
            }
            else{
                if (get_pointer(tail.both) == get_pointer(head.both)) return 0;
                else {
                    //TODO: add tag check and call fixlist
                    if(get_count(firstNodePrev.both) != get_count(head.both)){
                        fixList(Q,tail,head);
                        continue;
                    }
                    new_to_set = set_both(new_to_set,firstNodePrev.both,get_count(head.both)+(__int128)1);
                    int temp = __sync_bool_compare_and_swap(&(Q->Head.both),head.both,new_to_set);
                }
            }
        }
    }


            
}

void printqueue(struct queue_t * Q){

    struct pointer_t  curr ;
    struct pointer_t  prev;
    curr = Q->Head;
    prev = (get_pointer(Q->Head.both))->prev;
    while (get_pointer(curr.both) != get_pointer(Q->Tail.both)){
        printf("%d \n",(get_pointer(curr.both))->value);
        curr = prev;
        prev = (get_pointer(curr.both)) ->prev;
    }
    printf("%d ",(get_pointer(curr.both))->value);
    printf("\n");

}


int main(int argc,char * argv[]){

    int res,i,j,num_threads,count,val=0;
    struct queue_t * Q = (struct queue_t *) malloc(sizeof(struct queue_t));
    initialize(Q);

    num_threads = atoi(argv[1]);
    count = atoi(argv[2]);
    /*enqueue(Q,5);
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
    

    enqueue(Q,5);
    enqueue(Q,8);
    res= dequeue(Q,&val);
    if (res) printf("i dequeued %d \n",val);
    enqueue(Q,7);
    enqueue(Q,12);
    enqueue(Q,22);
    printqueue(Q);
*/

    srand(time(NULL));
    timer_tt * timer;
    int c,k;
    timer_tt * glob_timer=timer_init();
    timer_start(glob_timer);
    int sum=0;
    double total_time=0;


	#pragma omp parallel for num_threads(num_threads) shared(Q) private(res,val,i,j,c,timer,k) reduction(+:total_time) reduction(+:sum) 
	for(i=0;i<num_threads;i++){
         
        timer=timer_init();
        timer_start(timer);
        sum=0;
         for (j=0;j<count/num_threads;j++){
                enqueue(Q,i);
                c=rand()%1000;
                sum+=c;
                for(k=0;k<c;k++);
                res = dequeue(Q,&val);
                //if (res) printf("thread %d  dequeued --> %d\n",omp_get_thread_num(),val);
         }
         timer_stop(timer);
         total_time=timer_report_sec(timer);
         //printf("threads number %d total time %lf\n",omp_get_thread_num(),total_time);
         //printf("sum =  %ld\n",sum);
         /*timer_tt * timer2=timer_init();
         timer_start(timer2);
         for(k=0;k<sum;k++);
         timer_stop(timer2);
         total_time=timer_res-timer_report_sec(timer2);
         printf("delay time %lf\n",timer_report_sec(timer2));
         //printf("threads number %d  time %lf\n",omp_get_thread_num(),total_time);
         */
	}
   // printf("new total_time %lf\n",total_time);
   // printf("new sum %ld\n",sum);
    double avg_total_time=total_time/(double)num_threads;
    
    long int avg_sum=sum/num_threads;
   // printf("avg sum %ld\n",avg_sum);
    timer_tt * timer2=timer_init();
    timer_start(timer2);
    for(k=0;k<avg_sum;k++);
    timer_stop(timer2);
    double avg_delay=timer_report_sec(timer2);
   // printf("average delay time %lf\n",avg_delay);
    //printf("threads number %d  time %lf\n",omp_get_thread_num(),total_time);
         

    printf("average time %lf\n",avg_total_time - avg_delay);
    /*timer_stop(glob_timer);
    printf("global time %lf\n",timer_report_sec(glob_timer));

    glob_timer=timer_init();
    timer_start(glob_timer);
    for(i=0;i<sum;i++);
    timer_stop(glob_timer);
    printf("test delay %lf/n",timer_report_sec(glob_timer));
    */
	printqueue(Q);
    //timer_stop(timer);
    //printf("num_threasd %d  enq-deqs total %d \n",num_threads,count);
    //printf("Total time  %lf \n",time_res);
    

    return 1;
}
