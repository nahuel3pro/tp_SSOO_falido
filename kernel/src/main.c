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
    log_info(log, "%d", dictionary_get(dict, "LOG"));
    const char *path_to_psdc = argv[1];
    const int size = atoi(argv[2]);
    char *line;


    // Continuously call get_next_line until it returns NULL (EOF)
    while ((line = get_next_line(path_to_psdc)) != NULL)
    {
        log_info(log, "%s", line);
        readline("> ");
    }

    // Conectarse a memoria
    /* int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL); // Estaría bueno que sepa el server quién se conectó.
    readline("> "); */
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
