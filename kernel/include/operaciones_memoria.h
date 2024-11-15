#ifndef H_OPERACIONES_MEMORIA_H
#define H_OPERACIONES_MEMORIA_H

#include"globals.h"
#include "operaciones_kernel.h"

void process_create(char *filename, int process_size, int thread_priority);
void thread_create(int pid, int thread_priority, char *file_path);
void thread_cancel(int PID, int TID);
int connect_to_memory();


#endif