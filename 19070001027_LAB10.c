#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct PCB_s *PCB;
typedef struct PCB_s {
    int pid;
    int state; //state 0 means ready, state 1 means blocked
    int priority;
    PCB next_PCB_pointer;
    //additions
    char* address;
    char* name;
    char* type;
} PCB_t[1];

PCB init_PCB(int pid, int priority,int state,char* address,char* name,char* type) {
    PCB pcb = (PCB)malloc(sizeof(PCB_t));
    pcb->pid = pid;
    pcb->state = state;
    pcb->priority = priority;
    pcb->state = state;
    pcb->next_PCB_pointer = NULL;
    pcb->address = address;
    pcb->name = name;
    pcb->type = type;
    return pcb;
}


typedef struct LINKED_QUEUE_NODE_s *LINKED_QUEUE_NODE;
typedef struct LINKED_QUEUE_NODE_s{
    LINKED_QUEUE_NODE next;
    void *data;
} LINKED_QUEUE_NODE_t[1];

typedef struct LINKED_QUEUE_s *LINKED_QUEUE;
typedef struct LINKED_QUEUE_s{
    LINKED_QUEUE_NODE head;
    LINKED_QUEUE_NODE tail;
} LINKED_QUEUE_t[1];

LINKED_QUEUE linked_queue_init() {
    LINKED_QUEUE queue = (LINKED_QUEUE) malloc(sizeof(LINKED_QUEUE_t));
    if (queue != NULL) {
        queue->head = NULL;
        queue->tail = NULL;
    } else {
        printf("linked_queue_init: malloc failed!\n");
    }
    return queue;
}

int linked_queue_size(LINKED_QUEUE queue) {
    int size = 0;

    LINKED_QUEUE_NODE node = (LINKED_QUEUE_NODE) queue->head;
    while (node != NULL) {
        node = node->next;
        size++;
    }
    return size;
}

void linked_queue_free(LINKED_QUEUE queue) {
    free(queue);
}

int linked_queue_is_empty(LINKED_QUEUE queue) {
    return !(queue->head != NULL);
    // emptyse 0 degilse 1 returnluyor
}

void linked_queue_print(LINKED_QUEUE queue) {
    LINKED_QUEUE_NODE node = (LINKED_QUEUE_NODE) queue->head;
    printf("Queue --->");
    while (node != NULL) {
        int *value = node->data;
        printf(" %d ", value);
        node = node->next;
    }
    printf("\n");

}

int linked_queue_enqueue(LINKED_QUEUE queue, void *data)
{
    LINKED_QUEUE_NODE node;
    node = (LINKED_QUEUE_NODE)malloc(sizeof(LINKED_QUEUE_NODE_t));
    if(node != NULL) {
        node->data = data;
        node->next = NULL;
        if(queue->tail == NULL) {
            queue->head = node;
        }
        else {
            queue->tail->next = node;
        }
        queue->tail = node;
        return 1;
    }
    else {
        return 0;
    }
}

void* linked_queue_dequeue(LINKED_QUEUE queue)
{
    LINKED_QUEUE_NODE node;
    void *data;
    if(queue->head == NULL) {
        return  NULL;
    }
    else {
        if(queue->head == queue->tail) {
            queue->tail = NULL;
        }
        node = queue->head;
        queue->head = node->next;
        data = node->data;
        free(node);
        return  data;
    }
}

#define SIZE 5
PCB processTable[SIZE]; //PROCESS TABLE

int numberOFprocess=0;

int make_proc(int pid,int priority,int state,char* address,char* name,char* type,LINKED_QUEUE queue){
    numberOFprocess++;
    PCB pcb = init_PCB(pid,priority,state,address,name,type);

    if(numberOFprocess > 5){
        printf("Process table capacity is full ! You cant create new process");
    }else
    {
        processTable[numberOFprocess-1]=pcb;
    }

    linked_queue_enqueue(queue, (void *) pid);
    return pid;
}


PCB del_proc(LINKED_QUEUE queue){
    int pid = (int) linked_queue_dequeue(queue);
    PCB pcb = (PCB)malloc(sizeof(PCB_t));
    for (int i = 0; i < numberOFprocess; ++i)
    {
        if(pid == processTable[i]->pid)
        {
            *pcb = *processTable[i];
            free(processTable[i]);
        }
    }
    numberOFprocess--;
    return pcb;
}
void removeQueue(LINKED_QUEUE queue, int pid){

    LINKED_QUEUE_NODE node = (LINKED_QUEUE_NODE) queue->head;
    int index=0;
    while (node != NULL) {
        if(node->data == pid){
            break;
        }
        index++;
        node = node->next;
    }
    //QUEUEDAN REMOVE EDİLECEK ELEMANIN İNDEXİNİ BULDUK

    LINKED_QUEUE queue_another = linked_queue_init();

    for (int i = 0; i < index; ++i) {
        int pidd = (int) linked_queue_dequeue(queue);
        linked_queue_enqueue(queue_another, (void *) pidd);
    }
    // QUEUEDAN SİLİNECEK ELEMANA KADAR OLANALRIN HEPSİNİ DEQUEUE EDİP BAŞKA BİR ELEMANA ATIYORUZ

    int size_another = linked_queue_size(queue_another);//queuedan silmeyi de yaptıktan sonra diğer queueya yedekledıkklermızı gerı asıl queueya koyuyoruz
    for (int i = 0; i < size_another; ++i) {
        int pidd2001 = (int) linked_queue_dequeue(queue_another);
        linked_queue_enqueue(queue, (void *) pidd2001);
    }

    linked_queue_dequeue(queue);
}



int block_proc(int pid,LINKED_QUEUE queue){//block edilecek pidi bulup statesini değiştir
    for (int i = 0; i < SIZE; ++i) {
        if(pid == processTable[i]->pid){
            processTable[i]->state = 1 ; // 1 MEANS BLOCKED
        }
    }

    removeQueue(queue,pid); //BLOCKED STATESİ OLAN PCBYİ READY QUEUEDAN ÇIKARDIM

}

int unblock_proc(int pid,LINKED_QUEUE queue){//unblock edilecek pidi bulup statesini değiştiriyor
    for (int i = 0; i < SIZE; ++i) {

        if(pid == processTable[i]->pid)
        {
            processTable[i]->state = 0 ; // 1 MEANS UNBLOCKED
        }
    }

    linked_queue_enqueue(queue, (void *) pid);//UNBLOCKED OLAN PROCESİ TEKRAR READY QUEUEYE EKLEDİM

}

int main() {

    LINKED_QUEUE ready_queue = linked_queue_init();
    make_proc(1,1,0,"adress1","name1","type1",ready_queue);
    make_proc(2,2,0,"adress2","name2","type2",ready_queue);
    make_proc(3,3,0,"adress3","name3","type3",ready_queue);
    make_proc(4,4,0,"adress4","name4","type4",ready_queue);
    make_proc(5,1,0,"adress5","name5","type5",ready_queue);

    printf("\n I CHECKED IS MAKE_PROC FUNCTION WORKS OR NOT WITH PRINTING SOME OF VALUES\n");
    for (int i = 0; i < numberOFprocess; ++i) {
        printf("%s %s %s**",processTable[i]->address, processTable[i]->name,processTable[i]->type);
    }

    PCB test=del_proc(ready_queue);
    printf("\n\n\nI CHECKED DEL_PROC FUNCTION WITH DELETING FIRST PROCESS WHICH HAS PID = %d\n",test->pid);
    linked_queue_print(ready_queue);

    printf("AFTER DELETING, I CORRECTED MY QUEUE AND ENQUEUED DELETED PROC\n");

    make_proc(1,1,0,"adress1","name1","type1",ready_queue);
    linked_queue_print(ready_queue);


    printf("\n\nBLOCK AND UNBLOCK PROC TESTS\n");
    linked_queue_print(ready_queue);
    printf("process which pid equals 5 is deleted from ready queue because of its state blocked\n");
    block_proc(5,ready_queue);
    linked_queue_print(ready_queue);

    printf("process which pid equals 5 is enqueued to ready queue because of its state unblocked\n");
    unblock_proc(5,ready_queue);//5.processi block edince ready queuedan sildi
    linked_queue_print(ready_queue);
    printf("\n");

}
