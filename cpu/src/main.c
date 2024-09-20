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
        // comunicándose con memoria - Te pido el contexto de ejecución.
        // cpu manda el pid del proceso, así memoria lo busca y pasa el contexto de ejecución.
        t_paquete *paquete = malloc(sizeof(t_paquete));
        crear_buffer(paquete);
        recv(socket_cliente, &(paquete->op_code), SIZEOF_UINT8, 0);
        switch (paquete->op_code)
        {
        case GET_EXECUTION_CONTEXT:
            log_info(log, "Mandando contexto de ejecución...");
            break;
        case UPDATE_CONTEXT:
            log_info(log, "Actualizando contexto de ejecución...");
            break;
        case GET_INSTRUCTION:
            log_info(log, "Obteniendo instrucción de ejecución...");
            break;
        case READ_MEM:
            log_info(log, "Leyendo la memoria...");
            break;
        case WRITE_MEM:
            log_info(log, "Escribiendo en memoria...");
            break;

        default:
            break;
        }
    }
    else
    {
        error_show("error: ");
        log_error(log, "No se pudo conectar a memoria");
    }

    // readline("> ");
    return EXIT_SUCCESS;
}
