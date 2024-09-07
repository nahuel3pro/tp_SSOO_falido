#include "../include/cpu_tasks.h"

void *atenderCpu(void *void_args)
{
    log_info(log,"atendiendo cpu con memoria...");
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    // t_log *logger = args->log;
    int *socket_cpu_mem = args->fd;
    free(args);

    readline("> ");
    int fd_filesystem = crear_conexion(config_get_string_value(config, "IP_FILESYSTEM"), config_get_string_value(config, "PUERTO_FILESYSTEM"));
    if (send_handshake(log, fd_filesystem, "Memoria/Filesystem", MEMORIA))
    {
        log_info(log, "conectado a filesystem");
    }
    else{
        
    }
}
