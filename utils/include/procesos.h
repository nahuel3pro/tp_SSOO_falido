#ifndef PROCESOS_H_
#define PROCESOS_H_

#include <stdint.h>
#include <commons/collections/list.h>
#include <stdlib.h>
typedef enum
{
    HIGH,
    MEDIUM,
    LOW
} thrd_priority;

typedef enum
{
    NEW,
    READY,
    EXEC,
    BLOCKED
} process_state;
typedef struct
{
    uint32_t PC;
    uint32_t AX;
    uint32_t BX;
    uint32_t CX;
    uint32_t DX;
    uint32_t EX;
    uint32_t FX;
    uint32_t GX;
    uint32_t HX;
    uint32_t base;
    uint32_t limite;
} t_register;
typedef struct
{
    uint32_t PID;
    t_list *TIDs;
    process_state state;
    // t_list *mutex; Lo dejo comentado para cuando lo necesitemos.
    uint32_t size;
} *t_PCB;

typedef struct
{
    uint32_t TID;
    thrd_priority priority;
    t_list *instructions;
    t_register *registers;
} *t_TCB;

t_PCB pcb_create(int PID, int size);
t_TCB tcb_create(int TID, thrd_priority priority);
#endif