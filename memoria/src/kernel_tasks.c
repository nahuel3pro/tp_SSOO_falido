#include "../include/kernel_tasks.h"

void atenderKernel(void *void_args)
{
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    // t_log *logger = args->log;
    int *socket_kernel_mem = args->fd;
    free(args);

    log_info(log, "## Kernel Conectado - FD del socket: <%d>", *socket_kernel_mem);
    while (1)
    {
        int cod_op = recibir_operacion(socket_kernel_mem);
        switch (cod_op)
        {
        case PROCESS_CREATION:                                   // Crear protocolo de comunicación con kernel
            log_info(log, "Asignando espacio en memoria... OK"); // que reciba y muestre el tamaño del processo.
            break;
        case PROCESS_KILL:
            break;
        case THREAD_CREATION:
            break;
        case MEMORY_DUMP:
            break;
        default:
            log_warning(log, "Operacion desconocida. No quieras meter la pata");
            break;
        }
    }
    close(*socket_kernel_mem);
    free(socket_kernel_mem);
}
