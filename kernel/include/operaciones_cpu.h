#ifndef H_OPERACIONES_CPU_H_
#define H_OPERACIONES_CPU_H_

#include "globals.h"

void dispatch(t_TCB tcb, int fd);
void atender_motivo(char * motivo,t_buffer* buffer_response);

#endif