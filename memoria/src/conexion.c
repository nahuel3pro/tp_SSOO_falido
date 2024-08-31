#include "../include/conexion.h"

int recv_handshake_memoria(t_log *log, int server_fd)
{
    size_t bytes;

    int module;
    int32_t resultOk = 1;
    int32_t resultError = -1;
    bytes = recv(server_fd, &module, sizeof(int), MSG_WAITALL);
    if (module == KERNEL || module == CPU)
    {
        bytes = send(server_fd, &resultOk, sizeof(int32_t), 0);
        log_info(log, "Buenas tardes, caballero.");
        return module;
    }
    else
    {
        bytes = send(server_fd, &resultError, sizeof(int32_t), 0);
        log_info(log, "Che, se quiso meter un desconocido.");
        return -1;
    }
}