#ifndef H_OPERACIONES_MEMORIA_H
#define H_OPERACIONES_MEMORIA_H

#include"globals.h"

void process_create(char *filename, int process_size, int thread_priority);
void thread_create(int pid, int thread_priority, char *file_path);

#endif