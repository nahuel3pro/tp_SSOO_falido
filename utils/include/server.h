#ifndef UTILS_SERVER_H_
#define UTILS_SERVER_H_

//preguntar si tantos includes está bien o hay que reducir

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <string.h>

int iniciar_servidor(t_log * logger, char * puerto);
int esperar_cliente(t_log*, int server_fd);
int crear_conexion(char * ip, char * puerto);

#endif
