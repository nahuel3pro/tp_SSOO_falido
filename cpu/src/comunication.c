#include "../include/comunication.h"

void get_context(uint32_t PID, t_register *registro, int socket_conection)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    crear_buffer(paquete);

    // comunicándose con memoria - Te pido el contexto de ejecución.
    // cpu manda el pid del proceso, así memoria lo busca y pasa el contexto de ejecución.
    uint8_t pedido = (uint8_t)GET_EXECUTION_CONTEXT;
    send(socket_conection, &pedido, SIZEOF_UINT8, 0);
    // Se recibe el contexto de ejecución.
    recv(socket_conection, &(paquete->op_code), SIZEOF_UINT8, MSG_WAITALL);
    recv(socket_conection, &(paquete->buffer->size), SIZEOF_UINT32, 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);

    recv(socket_conection, paquete->buffer->stream, paquete->buffer->size, 0);
    deserealizar_registro(paquete->buffer, registro);

    eliminar_paquete(paquete);
}