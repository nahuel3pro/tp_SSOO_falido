#include "../include/main.h"

int main(int argc, char *argv[])
{
    t_config *config = NULL;
    t_log *log = NULL;
    int server_fd = 0;
    int client_fd = 0;

    config = levantar_config(getcwd(NULL, 0), "filesystem");
    log = levantar_log(getcwd(NULL, 0), "filesystem", config_get_string_value(config, "LOG_LEVEL"));

    // fileSystem como sv
    server_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA"));

    while ((client_fd = esperar_cliente(log, server_fd)) != -1)
    {
        // Nuevo socket de conexión para cada nuevo cliente
        int *client_connection = malloc(sizeof(client_fd));
        *client_connection = client_fd;
        
        if(recv_handshake_filesystem(log,*client_connection) == MEMORIA){
            atender_cliente(log, client_connection, atenderMemoria);
        }else{
            log_info(log, "Alguien no deseado quizo entrar");
            free(client_connection);
            close(client_fd);
        }
    }

    return EXIT_SUCCESS;
}
