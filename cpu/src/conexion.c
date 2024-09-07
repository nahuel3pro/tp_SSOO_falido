#include "../include/conexion.h"

int recv_handshake_cpu(t_log *logger, int client_fd)
{
    size_t bytes;

    int module;
    int32_t resultOk = 1;
    int32_t resultError = -1;
    bytes = recv(client_fd, &module, sizeof(int), MSG_WAITALL);
    log_info(logger, "El modulo -> %d",module);
    if (module == KERNEL)
    {
        bytes = send(client_fd, &resultOk, sizeof(int32_t), 0);
        return module;
    }
    else
    {
        bytes = send(client_fd, &resultError, sizeof(int32_t), 0);
        log_info(logger, "Che, se quiso meter un desconocido.");
        return -1;
    }
}

void servidor_dispatch()
{
    int server_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH"));

    while (1)
    {
        int client_fd = esperar_cliente(log, server_fd);
        // Nuevo socket de conección para cada nuevo cliente
        int *client_connection = malloc(sizeof(client_fd));
        *client_connection = client_fd;
        switch (recv_handshake_cpu(log, *client_connection))
        {
        case KERNEL:
            log_info(log, "Se conectó el kernel a dispatch");
            break;
        case -1:
            log_info(log, "Alguien no deseado quizo entrar");
            close(*client_connection);
            free(client_connection);
            break;
        default:
            break;
        }
        close(client_fd);
    }
}

void servidor_interrupt()
{
    int server_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));

    while (1)
    {
        int client_fd = esperar_cliente(log, server_fd);
        // Nuevo socket de conección para cada nuevo cliente
        int *client_connection = malloc(sizeof(client_fd));
        *client_connection = client_fd;
        switch (recv_handshake_cpu(log, *client_connection))
        {
        case KERNEL:
            log_info(log, "Se conectó el kernel a interrupt");
            break;
        case -1:
            log_info(log, "Alguien no deseado quizo entrar");
            close(*client_connection);
            free(client_connection);
            break;
        default:
            break;
        }
        close(*client_connection);
    }
}