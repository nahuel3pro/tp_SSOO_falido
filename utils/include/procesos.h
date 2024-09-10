#ifndef PROCESOS_H_
#define PROCESOS_H_

#include <stdint.h>
#include <commons/collections/list.h>

typedef enum
{
    ALTA,
    MEDIANA,
    BAJA
} prioridad;

typedef struct
{
    int32_t PID;
    t_list TID;
    t_list mutex;
} PCB;

typedef struct
{
    int32_t TID;
    prioridad p;
} TCB;

#endif