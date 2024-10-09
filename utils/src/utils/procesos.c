#include"../include/procesos.h"

t_PCB pcb_create(int PID, int size){
    //  Creación de proceso con PID
    t_PCB pcb = malloc(sizeof(t_PCB));
    pcb->PID = (uint32_t)PID;
    pcb->size = (uint32_t)size;
    pcb->TIDs = list_create();
    // faltan los mutex;

    return pcb;
}

t_TCB tcb_create(int TID, int priority, char* file_path){
    t_TCB tcb = malloc(sizeof(t_TCB));
    tcb->TID = (uint32_t)TID;
    tcb->priority = priority;

    return tcb;
}