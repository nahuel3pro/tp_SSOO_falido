#include "../include/main.h"

t_config *config = NULL;
t_log *log = NULL;

int main(int argc, char *argv[])
{
    t_list* instrucciones_kernel = list_create();
    //config = levantar_config(getcwd(NULL, 0), "cpu");
    //log = levantar_log(getcwd(NULL, 0), "cpu", config_get_string_value(config, "LOG_LEVEL"));
    int server_fd = 0;
    //int client_fd = 0;

    config = levantar_config(getcwd(NULL, 0), "cpu");
    log = levantar_log(getcwd(NULL, 0), "cpu", config_get_string_value(config, "LOG_LEVEL"));

    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));

    if (send_handshake(log, socket_cliente, "CPU/memoria", CPU))
    {
        // CPU como server - interrupt
        server_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));

        while (1)
        {
            client_fd = esperar_cliente(log, server_fd);

            switch (recv_handshake_cpu(log, client_fd))
            {
            case KERNEL:
                log_info(log, "Se conectó el kernel");
                instrucciones_kernel = recibir_paquete(client_fd);
                int i = 0;
                while(!list_is_empty(instrucciones_kernel)){
                    i++;
                    char*elemento = list_remove(instrucciones_kernel,0);
                    printf("instruccion %d : %s",i,elemento);
                }
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
    }
    else
    {
        error_show("error: ");
        log_error(log, "No se pudo conectar a memoria");
    } 

    pthread_t thread_dispatch;
    pthread_t thread_interrupt;
    pthread_create(&thread_dispatch, NULL, servidor_dispatch, NULL);
    pthread_create(&thread_interrupt, NULL, servidor_interrupt, NULL);
    pthread_detach(thread_dispatch);
    pthread_detach(thread_interrupt);

    //readline("> ");
    return EXIT_SUCCESS;
}
