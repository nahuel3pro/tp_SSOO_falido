#include "../include/main.h"
#include "../include/operaciones_kernel.h"

int main(int argc, char *argv[])
{
    t_config *config = NULL;
    t_log *log = NULL;
    t_paquete* paquete_instrucciones = crear_paquete();

    config = levantar_config(getcwd(NULL, 0), "kernel");
    log = levantar_log(getcwd(NULL, 0), "kernel", config_get_string_value(config, "LOG_LEVEL"));

    t_list* instrucciones = recibir_instrucciones("test.dat");

    while(!list_is_empty(instrucciones)){
        char*elemento = list_remove(instrucciones,0);
        agregar_a_paquete(paquete_instrucciones,elemento,strlen(elemento+1));
        printf("%s\n",elemento);
        free(elemento);
    }

    // Conectarse a memoria
            /* int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
            send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL); // Estaría bueno que sepa el server quién se conectó.
            readline("> "); */
    // Conectarse a cpu dispatch
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
    send_handshake(log, socket_cliente, "Kernel/CPU_dispatch", KERNEL);
    enviar_paquete(paquete_instrucciones,socket_cliente);
    // // Conectarse a cpu interrupt
    /* int socket_cliente = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
    send_handshake(log, socket_cliente, "Kernel/CPU_interrupt", KERNEL);
 */

    return 0;
}
