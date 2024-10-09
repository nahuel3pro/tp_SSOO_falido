#ifndef KERNEL_TASKS_H_
#define KERNEL_TASKS_H_

#include "main.h"
#include <commons/log.h>
#include <commons/config.h>
#include <readline/readline.h>

void atenderKernel(void *void_args);
void load_list_instructions(t_list *list_instructions,char* path);
void initiate_registers(t_register *registro);

#endif