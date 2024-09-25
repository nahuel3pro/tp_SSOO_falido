#include "../include/comunication.h"

void get_context(uint32_t PID, uint32_t TID, t_register *registro, int socket_connection)
{

    // comunicándose con memoria - Te pido el contexto de ejecución.
    // cpu manda el pid del proceso, así memoria lo busca y pasa el contexto de ejecución.
    // Mando(serializar) TID y PID
    t_paquete *paquete_send = crear_paquete(GET_EXECUTION_CONTEXT);
    t_buffer *buffer = buffer_create(SIZEOF_UINT32 * 2);
    buffer_add_uint32(buffer, PID);
    buffer_add_uint32(buffer, TID);
    buffer->offset = 0;
    paquete_send->buffer = buffer;

    enviar_paquete(paquete_send, socket_connection);
    eliminar_paquete(paquete_send);

    // Se recibe el contexto de ejecución.
    t_buffer *buffer_r;
    buffer_r = malloc(sizeof(t_buffer));
    buffer_recv(socket_connection, buffer_r);

    deserealizar_registro(buffer_r, registro);
}
