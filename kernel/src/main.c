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
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL); // Estaría bueno que sepa el server quién se conectó.

    uint8_t p = (uint8_t)PROCESS_KILL;
    // send(socket_cliente, &p, sizeof(uint8_t), 0);
    //  proceso 0
    t_PCB pcb = malloc(sizeof(t_PCB));
    pcb->PID = 1;
    // pcb->TIDs = list_create();
    pcb->size = process_size;
    // thread 0
    t_TCB thread = malloc(sizeof(t_TCB));
    thread->priority = HIGH;
    thread->TID = 0;
    // Serializar el proceso.
    t_buffer *buffer = serializarProceso(pcb);
    // empaquetar
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->op_code = (uint8_t)PROCESS_CREATION;
    paquete->buffer = buffer;

    // Armamos el stream a enviar
    void *a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
    int offset = 0;

    memcpy(a_enviar + offset, &(paquete->op_code), sizeof(uint8_t));

    offset += sizeof(uint8_t);
    memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

    // Por último enviamos
    send(socket_cliente, a_enviar, buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0);

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
