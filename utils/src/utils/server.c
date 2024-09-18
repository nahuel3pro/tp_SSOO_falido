#include "../include/server.h"

int iniciar_servidor(t_log *logger, char *puerto_escucha)
{
    int yes = 1;
    int socket_servidor;
    // sockaddr_in = addrinfo
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, puerto_escucha, &hints, &servinfo);

    // Creamos el socket de escucha del servidor

    if ((socket_servidor = socket(servinfo->ai_family,
                                  servinfo->ai_socktype,
                                  servinfo->ai_protocol)) == -1)
    {
        error_show("Error creando el servidor");
        abort();
    };
    // int err = setsockopt(fd_escucha, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        error_show("Setsockopt error");
        abort();
    }
    /* Esta linea la usaremos más tarde */

    // Asociamos el socket a un puerto
    if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        error_show("Error al bindear servidor");
        abort();
    };

    // Escuchamos las conexiones entrantes

    listen(socket_servidor, SOMAXCONN);
    log_trace(logger, "Listo para escuchar a mi cliente");

    freeaddrinfo(servinfo);

    return socket_servidor;
}

int esperar_cliente(t_log *logger, int server_fd)
{
    // Aceptamos un nuevo cliente
    int socket_cliente = accept(server_fd, NULL, NULL);

    if (socket_cliente == -1)
    {
        log_error(logger, "Error al conectar un cliente!");
        return socket_cliente;
    }
    log_info(logger, "Se conectó un cliente");

    return socket_cliente;
}

int crear_conexion(char *ip, char *puerto)
{
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &server_info);

    // Ahora vamos a crear el socket.
    int socket_cliente = 0;
    socket_cliente = socket(server_info->ai_family,
                            server_info->ai_socktype,
                            server_info->ai_protocol);

    // Ahora que tenemos el socket, vamos a conectarlo
    if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen))
    {
        abort();
    }

    freeaddrinfo(server_info);

    return socket_cliente;
}

void atender_cliente(t_log *log, int *client_fd, void (*func)(void))
{
    pthread_t hilo;
    t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
    args->log = log;
    args->fd = client_fd;
    pthread_create(&hilo, NULL, func, (void *)args);
    pthread_detach(hilo);
}

uint8_t recibir_operacion(int socket_cliente)
{
	uint8_t cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(uint8_t), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}