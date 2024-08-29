#include "../include/protocolo.h"

bool send_handshake(t_log *logger, int fd_connection, const char *connection_name)
{
    size_t bytes;
    int32_t handshake = 1;
    int32_t result = 0;

    bytes = send(fd_connection, &handshake, sizeof(int32_t), 0);
    bytes = recv(fd_connection, &result, sizeof(int32_t), MSG_WAITALL);

    if (result == 1)
    {
        log_info(logger, "Handshake OK de %s, bienvenido", connection_name);
    }
    else
    {
        log_error(logger, "Handshake ERROR, no te conozco");
        close(fd_connection);
    }
    return true;
}

bool recv_handshake(t_log *log, int server_fd)
{
    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 1;
    int32_t resultError = -1;

    bytes = recv(server_fd, &handshake, sizeof(int32_t), MSG_WAITALL);
    if (handshake == 1)
    {
        bytes = send(server_fd, &resultOk, sizeof(int32_t), 0);
        log_info(log, "Buenas tardes, caballero.");
    }
    else
    {
        bytes = send(server_fd, &resultError, sizeof(int32_t), 0);
        log_info(log, "Che, se quiso meter un desconocido.");
    }
    return true;
}

t_buffer *buffer_create(void)
{
    // Creo y seteo las variables del buffer
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = 0;
    buffer->stream = NULL;

    return buffer;
}