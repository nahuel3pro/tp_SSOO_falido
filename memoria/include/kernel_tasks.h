#ifndef KERNEL_TASKS_H_
#define KERNEL_TASKS_H_

#include "main.h"
#include <commons/log.h>
#include <commons/config.h>
#include <readline/readline.h>

void atenderKernel(void *void_args);
void process_create(int socket_kernel_mem);
void thread_create(int socket_kernel_mem);

#endif