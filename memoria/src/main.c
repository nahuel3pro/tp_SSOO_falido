#include "../include/main.h"

int main(int argc, char *argv[])
{
    t_config *config = NULL;
    t_log *log = NULL;
    int server_fd = 0;
    int client_fd = 0;
    int module_conexion;

    config = levantar_config(getcwd(NULL, 0), "memoria");
    log = levantar_log(getcwd(NULL, 0), "memoria", config_get_string_value(config, "LOG_LEVEL"));

    server_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA"));

    // readline("> ");

    while (1)
    {
        client_fd = esperar_cliente(log, server_fd);
        switch (recv_handshake_memoria(log, client_fd))
        {
        case CPU:
            log_info(log, "Se conectó el cpu");
            break;
        case KERNEL:
            log_info(log, "Se conectó el kernel");
            break;
        case -1:
            log_info(log, "Alguien no deseado quizo entrar");
            break;
        default:
            break;
        }
        }

    return EXIT_SUCCESS;
}
