#include"../include/procesos.h"

t_PCB pcb_create(int PID, int size){
    //  proceso 0
    t_PCB pcb = malloc(sizeof(t_PCB));
    pcb->PID = (uint32_t)PID;
    pcb->size = (uint32_t)size;
    pcb->TIDs = list_create();
    // thread 0
    t_TCB initial_thread = tcb_create(0, HIGH);
    list_add(pcb->TIDs, initial_thread);

    return pcb;
}

t_TCB tcb_create(int TID, thrd_priority priority){
    t_TCB tcb = malloc(sizeof(t_TCB));
    tcb->TID = (uint32_t)TID;
    tcb->priority = HIGH;

    return tcb;
}