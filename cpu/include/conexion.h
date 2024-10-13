#ifndef CONEXION_H_
#define CONEXION_H_

#include "globals.h"
#include "../include/protocolo.h"

int recv_handshake_cpu(t_log *log, int server_fd);
void servidor_dispatch();
void servidor_interrupt();

#endif
