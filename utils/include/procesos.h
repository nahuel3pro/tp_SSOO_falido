#ifndef PROCESOS_H_
#define PROCESOS_H_

#include <stdint.h>
#include <commons/collections/list.h>
typedef enum{
    HIGH,
    MEDIUM,
    LOW
} thrd_priority;

typedef struct{
    uint32_t PID;
    //t_list *TIDs;
    //t_list *mutex; Lo dejo comentado para cuando lo necesitemos.
    uint32_t size;
} *t_PCB;

typedef struct{
    uint32_t TID;
    thrd_priority priority;
} *t_TCB;
#endif