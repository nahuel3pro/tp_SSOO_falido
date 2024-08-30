#include "../include/main.h"

int main(int argc, char *argv[])
{
    t_config *config = NULL;
    t_log *log = NULL;
    int server_fd = 0;
    int client_fd = 0;

    config = levantar_config(getcwd(NULL, 0), "memoria");
    log = levantar_log(getcwd(NULL, 0), "memoria", config_get_string_value(config, "LOG_LEVEL"));

    // Memoria como sv
    server_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA"));

    while ((client_fd = esperar_cliente(log, server_fd)) != -1)
    {
        switch (recv_handshake_memoria(log, client_fd))
        {
        case KERNEL:
            log_info(log, "Se conectó el kernel");
            break;
        case CPU:
            log_info(log, "Se conectó el cpu");
            break;
        case -1:
            log_info(log, "Alguien no deseado quizo entrar");
            close(client_fd);
            break;
        default:
            break;
        }
        close(client_fd);
    }

    // Memoria como cliente

/*     int servidor_fd = crear_conexion(config_get_string_value("IP_FILESYSTEM"), config_get_string_value("PUERTO_FILESYSTEM"));
    if (send_handshake(log, servidor_fd, "Memoria/Filesystem", KERNEL))
    {
        log_info(log, "Comenzando operaciones con filesystem...");
        readline("> ");
    } */

    return EXIT_SUCCESS;
}
