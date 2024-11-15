#include "../include/main.h"
t_config *config;
t_log *log;
t_list *process_list;
pthread_mutex_t mutex_process_list;

int main(int argc, char *argv[])
{
    var_initiate();

    int server_fd = 0;
    int client_fd = 0;

    // Memoria como sv
    server_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA"));

    while ((client_fd = esperar_cliente(log, server_fd)) != -1)
    {
        // Nuevo socket de conexión para cada nuevo cliente
        int *client_connection = malloc(sizeof(client_fd));
        *client_connection = client_fd;
        switch (recv_handshake_memoria(log, *client_connection))
        {
        case KERNEL:
            atender_cliente(log, client_connection, atenderKernel);
            break;
        case CPU:
            atender_cliente(log, client_connection, atenderCpu);
            break;
        case -1:
            log_info(log, "Alguien no deseado quizo entrar");
            free(client_connection);
            close(client_fd);
            break;
        default:
            break;
        }
    }

    return EXIT_SUCCESS;
}

void var_initiate(void)
{
    config = levantar_config(getcwd(NULL, 0), "memoria");
    log = levantar_log(getcwd(NULL, 0), "memoria", config_get_string_value(config, "LOG_LEVEL"));
    process_list = list_create();
    pthread_mutex_init(&mutex_process_list, NULL);
}