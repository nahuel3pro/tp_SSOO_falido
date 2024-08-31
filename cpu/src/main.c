#include "../include/main.h"

int main(int argc, char *argv[])
{
    t_config *config = NULL;
    t_log *log = NULL;
    int server_fd = 0;
    int client_fd = 0;

    config = levantar_config(getcwd(NULL, 0), "cpu");
    log = levantar_log(getcwd(NULL, 0), "cpu", config_get_string_value(config, "LOG_LEVEL"));

    // CPU como server
/*     server_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));

    while (1)
    {
        client_fd = esperar_cliente(log, server_fd);

        switch (recv_handshake_cpu(log, client_fd))
        {
        case KERNEL:
            log_info(log, "Se conectó el kernel");
            break;
        case -1:
            log_info(log, "Alguien no deseado quizo entrar");
            close(client_fd);
            break;
        default:
            break;
        }
        close(client_fd);
    } */

    // CPU como cliente
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "CPU/memoria", CPU); 


    return EXIT_SUCCESS;
}
