#ifndef UTILS_PROTOCOLO_H_
#define UTILS_PROTOCOLO_H_

#include <stdlib.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

typedef enum
{
    KERNEL,
    CPU,
    MEMORIA,
    FILESYSTEM
} op_module;

typedef struct
{
    uint32_t size;   // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void *stream;    // Payload
} t_buffer;

	/**
	* @brief Intenta entrar en contacto con un servidor.
	* @param logger loger.
    * @param fd Socket de conexión del cliente.
    * @param connection_name Modulos que establecen su conexión ej: Kernel/Memoria.
	* @param module constante enum del propio cliente para que el servidor identifique.
    * @return false si no se aceptó, true si se aceptó la conexión.
	*/
bool send_handshake(t_log *logger, int fd, const char *connection_name, int module);

bool recv_handshake(t_log *logger, int server_fd);
t_buffer *buffer_create();

#endif