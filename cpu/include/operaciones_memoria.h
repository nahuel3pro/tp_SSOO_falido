#ifndef H_OPERACIONES_MEMORIA_H
#define H_OPERACIONES_MEMORIA_H

#include "globals.h"
#include "comunication.h"
#include "tools.h"

char *recv_instruction(int socket, uint32_t PID, uint32_t TID, uint32_t PC);

char *fetch(int socket_cliente, t_register *registro, uint32_t PID, uint32_t TID);
void decode_execute(char *instruction, t_register *registro, uint32_t PID, uint32_t TID);
#endif