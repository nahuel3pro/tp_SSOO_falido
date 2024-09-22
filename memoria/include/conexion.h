#ifndef CONEXION_H_
#define CONEXION_H_

#include "../include/protocolo.h"
#include "../include/globals.h"
#include <commons/config.h>

	/**
	* @brief Examina el módulo que está intentando conectarse al servidor.
	* @param log logger.
    * @param server_fd file descriptor.
	* @return Retorna un entero con los siguentes valores.
    * @code
	* [0] -> El módulo del cliente es kernel.
    * [1] -> El módulo del cliente es CPU.
    * [-1] -> El cliente que se quizo conectar es desconocido.
	* @endcode
	*/
int recv_handshake_memoria(t_log* log, int server_fd);
void wait();

#endif

