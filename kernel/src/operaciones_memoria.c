#include "../include/operaciones_memoria.h"

void process_create(int PID, int process_size)
{
    while(!queue_is_empty(new_queue)){
    // Conectarse a memoria
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL);

    t_paquete *paquete_send = crear_paquete(PROCESS_CREATION);
    t_buffer *buffer_send = buffer_create(SIZEOF_UINT32 * 2);
    buffer_add_uint32(buffer_send, (uint32_t)PID);
    buffer_add_uint32(buffer_send, (uint32_t)process_size);
    buffer_send->offset = 0;

    paquete_send->buffer = buffer_send;
    enviar_paquete(paquete_send, socket_cliente);
    eliminar_paquete(paquete_send);

    int res = 1;
    // Se espera la respuesta positiva de memoria para poder mandarlo a la cola de READY
    while(res == FAILURE){
        recv(socket_cliente, &res, SIZEOF_UINT32, MSG_WAITALL);
    }

    
    }

}