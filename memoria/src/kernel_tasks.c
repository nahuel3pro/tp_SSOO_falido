#include "../include/kernel_tasks.h"

void atenderKernel(void *void_args)
{
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    // t_log *logger = args->log;
    int *socket_kernel_mem = args->fd;
    free(args);

    log_info(log, "## Kernel Conectado - FD del socket: <%d>", *socket_kernel_mem);
    readline("> ");
    close(*socket_kernel_mem);
    free(socket_kernel_mem);
}
