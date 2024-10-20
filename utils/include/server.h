#ifndef UTILS_SERVER_H_
#define UTILS_SERVER_H_

// preguntar si tantos includes está bien o hay que reducir

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/error.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

typedef struct
{
    t_log *log;
    int *fd;
} t_procesar_conexion_args;

int iniciar_servidor(t_log *logger, char *puerto);
int esperar_cliente(t_log *, int server_fd);
int crear_conexion(char *ip, char *puerto);

void* recibir_buffer(int*, int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
uint8_t recibir_operacion(int);

	/**
	* @brief Crea un pthread_detach para atender el cliente entrante.
	* @param log logger.
    * @param client_fd puntero al socket de conexión entre cliente y sv.
    * @param func función a ejecutar.
	* @return void.
	*/
void atender_cliente(t_log *log, int *client_fd, void (*func)(void));

#endif
