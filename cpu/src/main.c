#include "../include/main.h"

t_config *config = NULL;
t_log *log = NULL;

int main(int argc, char *argv[])
{
    t_list *instrucciones_kernel = list_create();
    // config = levantar_config(getcwd(NULL, 0), "cpu");
    // log = levantar_log(getcwd(NULL, 0), "cpu", config_get_string_value(config, "LOG_LEVEL"));
    int server_fd = 0;
    int client_fd = 0;

    config = levantar_config(getcwd(NULL, 0), "cpu");
    log = levantar_log(getcwd(NULL, 0), "cpu", config_get_string_value(config, "LOG_LEVEL"));

    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));

    // Supongamos que nos llega el pedido de kernel para ejecutar un proceso

    if (send_handshake(log, socket_cliente, "CPU/memoria", CPU))
    {
        /*  t_register *registro = malloc(sizeof(t_register));
         get_context(1, registro, socket_cliente); */
        char* instruction;
        t_register registro;
       
        fetch(socket_cliente, instruction, &registro, 1, 0);
        decode_execute(instruction, &registro, 1, 0);

        readline(">");
    }

    // readline("> ");
    return EXIT_SUCCESS;
}
