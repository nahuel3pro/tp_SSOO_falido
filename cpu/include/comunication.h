#ifndef H_COMUNICATION_H_
#define H_COMUNICATION_H_

#include "../../utils/include/procesos.h"
#include "../../utils/include/protocolo.h"
#include "../../utils/include/hello.h"

void get_context(uint32_t PID, t_register *registro, int socket_conection);

#endif