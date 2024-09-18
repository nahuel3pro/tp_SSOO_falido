#include "../include/kernel_tasks.h"

void atenderKernel(void *void_args)
{
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    // t_log *logger = args->log;
    int *socket_kernel_mem = args->fd;
    free(args);

    log_info(log, "## Kernel Conectado - FD del socket: <%d>", *socket_kernel_mem);

    // uint8_t cod_op = recibir_operacion(*socket_kernel_mem);
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->buffer = malloc(sizeof(t_buffer));
    // Primero recibimos el codigo de operacion
    recv(socket_kernel_mem, &(paquete->op_code), sizeof(uint8_t), 0);
    // Después ya podemos recibir el buffer.Primero su tamaño seguido del contenido
    recv(socket_kernel_mem, &(paquete->buffer->size), sizeof(uint32_t), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket_kernel_mem, paquete->buffer->stream, paquete->buffer->size, 0);
    switch ((int)paquete->op_code)
    {
    case PROCESS_CREATION:                                   // Crear protocolo de comunicación con kernel
        log_info(log, "Asignando espacio en memoria... OK"); // que reciba y muestre el tamaño del processo.
        break;
    case PROCESS_KILL:
        log_info(log, "Matando el proceso");
        break;
    case THREAD_CREATION:
        break;
    case MEMORY_DUMP:
        break;
    default:
        log_warning(log, "Operacion desconocida. No quieras meter la pata");
        break;
    }
    close(*socket_kernel_mem);
    free(socket_kernel_mem);
}
