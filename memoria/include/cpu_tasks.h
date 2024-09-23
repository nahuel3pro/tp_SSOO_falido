#ifndef CPU_TASKS_H_
#define CPU_TASKS_H_

#include "main.h"
#include"../include/server.h"
#include"../include/tools.h"
#include<commons/log.h>
#include<commons/config.h>

void atenderCpu(void* void_args);
void execution_context(int socket);
void get_instruction(int socket);

#endif