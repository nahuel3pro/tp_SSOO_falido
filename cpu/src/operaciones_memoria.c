#include"../include/operaciones_memoria.h"


// Recive el pedido de instrucción hecho anteriormente.
char *recv_instruction(int socket_cliente)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    crear_buffer(paquete);
    recv(socket_cliente, &paquete->op_code, SIZEOF_UINT8, 0);
    recv(socket_cliente, &paquete->buffer->size, SIZEOF_UINT32, 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket_cliente, paquete->buffer->stream, paquete->buffer->size, 0);
    // Se recibe el contexto de ejecución.
    uint32_t str_size;
    char *inst = buffer_read_string(paquete->buffer, &str_size);

    return inst;
}