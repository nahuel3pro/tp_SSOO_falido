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
    int32_t PID;
    t_list *TIDs;
    //t_list *mutex; Lo dejo comentado para cuando lo necesitemos.
    int32_t size;
} *PCB;

typedef struct{
    int TID;
    thrd_priority priority;
} *TCB;
#endif