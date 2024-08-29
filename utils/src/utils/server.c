#include "../include/server.h"

int iniciar_servidor(t_log *logger, char *puerto_escucha)
{
    int err;
    int socket_servidor;
    // sockaddr_in = addrinfo
    struct addrinfo hints, *servinfo, *p; // Para qué se usa *p?

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
    connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

    freeaddrinfo(server_info);

    return socket_cliente;
}