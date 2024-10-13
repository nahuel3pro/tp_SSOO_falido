#ifndef H_OPERACIONES_KERNEL_H
#define H_OPERACIONES_KERNEL_H

#include "globals.h"
#include "comunication.h"
#include "tools.h"
#include "operaciones_memoria.h"

void atenderKernel();
void atenter_dispatch(void);
void dispatch(int client_dispatch_fd);


void atender_interrupt(void);
#endif