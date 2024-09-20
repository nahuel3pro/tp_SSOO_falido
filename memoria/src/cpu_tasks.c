#include "../include/cpu_tasks.h"

void atenderCpu(void *void_args)
{
    log_info(log, "atendiendo cpu con memoria...");
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    // t_log *logger = args->log;
    int *socket_cpu_mem = args->fd;
    free(args);

    while (1)
    {
        t_paquete *paquete = malloc(sizeof(t_paquete));
        crear_buffer(paquete);
        recv(*socket_cpu_mem, &(paquete->op_code), SIZEOF_UINT8, 0);
        switch (paquete->op_code)
        {
        case GET_EXECUTION_CONTEXT:
            log_info(log, "Mandando contexto de ejecución...");
            break;
        case UPDATE_CONTEXT:
            log_info(log, "Actualizando contexto de ejecución...");
            break;
        case GET_INSTRUCTION:
            log_info(log, "Obteniendo instrucción de ejecución...");
            break;
        case READ_MEM:
            log_info(log, "Leyendo la memoria...");
            break;
        case WRITE_MEM:
            log_info(log, "Escribiendo en memoria...");
            break;
        default:
            // lo que me mandaste no tiene sentido
            break;
        }
    }
