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
        // Nuevo socket de conección para cada nuevo cliente
        /*  int *client_connection = new(sizeof(client_fd));
         *client_connection = client_fd; */
        switch (recv_handshake_memoria(log, client_fd))
        {
        case KERNEL:
            /*  log_info(log, "Se conectó el kernel");
             pthread_t hilo;
             t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
             args->log = log;
             args->fd = client_fd;
             args->server_name = "Kernel/Memoria";
             pthread_create(&hilo, NULL, atendercpu, (void *)args);
             pthread_detach(hilo); */
            atender_cliente(log, client_fd, atenderKernel);
            break;
        case CPU:
            atender_cliente(log, client_fd, atenderCpu);
            break;
        case -1:
            log_info(log, "Alguien no deseado quizo entrar");
            close(client_fd);
            break;
        default:
            break;
        }
    }

    // Memoria como cliente -- Pendiente

    /*     int servidor_fd = crear_conexion(config_get_string_value("IP_FILESYSTEM"), config_get_string_value("PUERTO_FILESYSTEM"));
        if (send_handshake(log, servidor_fd, "Memoria/Filesystem", KERNEL))
        {
            log_info(log, "Comenzando operaciones con filesystem...");
            readline("> ");
        } */

    return EXIT_SUCCESS;
}
