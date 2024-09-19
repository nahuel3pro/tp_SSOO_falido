#include "../include/main.h"
t_config *config = NULL;
t_log *log = NULL;
t_dictionary *dict = NULL;

int main(int argc, char *argv[])
{
    // Definiendo herramientas.
    config = levantar_config(getcwd(NULL, 0), "kernel");
    log = levantar_log(getcwd(NULL, 0), "kernel", config_get_string_value(config, "LOG_LEVEL"));
    dict = dict_protocol();
    const char *path_to_psdc = argv[1];
    const int process_size = atoi(argv[2]);
    char *line;

    // Continuously call get_next_line until it returns NULL (EOF)
    // while ((line = get_next_line(path_to_psdc)) != NULL)
    // {
    //     log_info(log, "%s", line);
    //     readline("> ");
    // }

    // Conectarse a memoria
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL);

    //  proceso 0
    t_PCB pcb = pcb_create(1,process_size);

    // Serializar el proceso. Buffer con los datos del PCB--- Faltan datos.
    t_buffer *buffer = serializarProceso(pcb);         // --- Faltan las listas.
    buffer->offset = 0;

    // empaquetar --------------- enviar pcb
    if (send_pcb(pcb, PROCESS_CREATION, buffer, socket_cliente) > 0)
    {
        log_info(log, "Proceso enviado");
        uint8_t res;
        if (recv(socket_cliente, &res, SIZEOF_UINT8, MSG_WAITALL) > 0 && res == (uint8_t)0)
        {
            log_info(log, "Proceso cargado exitosamente!");
        }
        else
        {
            log_error(log, "No hay memoria disponible, volviendo a la cola de ready...");
        }
    }
    else
    {
        log_error(log, "no se pudo mandar el proceso");
    }

    readline(">");
    // Conectarse a cpu dispatch
    /* int socket_cliente = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
    send_handshake(log, socket_cliente, "Kernel/CPU_dispatch", KERNEL);
    enviar_paquete(paquete_instrucciones, socket_cliente); */
    // // Conectarse a cpu interrupt
    /* int socket_cliente = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
    send_handshake(log, socket_cliente, "Kernel/CPU_interrupt", KERNEL);
 */

    return 0;
}
