#ifndef H_TOOLS_H
#define H_TOOLS_H

#include "globals.h"
#include <stdio.h>
#include "main.h"

char *get_next_line(const char *filename);
void initiate_registers(t_register *my_register);
void asignar_algoritmo(char *algoritmo);
bool thread_is_waiting(int pid, int tid);
t_PCB get_process(int PID);
t_TCB get_thread(int PID, int TID);

#endif