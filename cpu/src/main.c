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
    t_paquete *paquete = malloc(sizeof(t_paquete));
    crear_buffer(paquete);

    if (send_handshake(log, socket_cliente, "CPU/memoria", CPU))
    {
        // comunicándose con memoria - Te pido el contexto de ejecución.
        // cpu manda el pid del proceso, así memoria lo busca y pasa el contexto de ejecución.
        uint32_t pedido = (uint32_t)GET_EXECUTION_CONTEXT;
        send(socket_cliente, &pedido, SIZEOF_UINT32, 0);
        // Se recibe el contexto de ejecución.
        t_register *registro = malloc(sizeof(t_register));
        recv(socket_cliente, &(paquete->buffer->size), SIZEOF_UINT32, MSG_WAITALL);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(socket_cliente, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);
        deserealizar_registro(paquete->buffer, registro);
    }


    error_show("error: ");
    log_error(log, "No se pudo conectar a memoria");


// readline("> ");
return EXIT_SUCCESS;
}
