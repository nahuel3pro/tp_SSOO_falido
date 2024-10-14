#ifndef H_TOOLS_H_
#define H_TOOLS_H_

#include <stdio.h>
#include "globals.h"

char *get_next_line(const char *filename);
void load_list_instructions(t_list *list_instructions, const char *path);
void initiate_registers(t_register *registro);
t_TCB get_thread(int PID, int TID);
t_TCB thread_initiate(char *file_path, int thread_priority, int PID, int TID);
t_PCB get_process(int PID);
t_PCB process_initiate(int PID, int size);
void update_registers(t_register *mem_reg, t_register new_registers);
#endif