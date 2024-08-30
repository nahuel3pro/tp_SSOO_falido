#include "../include/main.h"

int main(int argc, char *argv[])
{
    t_config *config = NULL;
    t_log *log = NULL;

    config = levantar_config(getcwd(NULL, 0), "kernel");
    log = levantar_log(getcwd(NULL, 0), "kernel", config_get_string_value(config, "LOG_LEVEL"));

    // Conectarse a memoria
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL); // Estaría bueno que sepa el server quién se conectó.
    readline("> ");
    // Conectarse a cpu dispatch
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
    send_handshake(log, socket_cliente, "Kernel/CPU_dispatch", KERNEL); 
    // Conectarse a cpu interrupt 
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
    send_handshake(log, socket_cliente, "Kernel/CPU_interrupt", KERNEL); 


    return 0;
}
