#include "../include/cpu_tasks.h"

void atenderCpu(void *void_args)
{
    log_info(log, "atendiendo cpu con memoria...");
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    // t_log *logger = args->log;
    int *socket_cpu_mem = args->fd;
    free(args);

    // memoria pide este registro
    t_register registro;
    registro.PC = 3;
    registro.AX = 80;
    registro.BX = 44;
    registro.CX = 2;
    registro.EX = 99;

    while (1)
    {
        t_paquete *paquete = malloc(sizeof(t_paquete));
        crear_buffer(paquete);
        recv(*socket_cpu_mem, &(paquete->op_code), SIZEOF_UINT8, 0);
        switch (paquete->op_code)
        {
        case GET_EXECUTION_CONTEXT:
            // recibir el pid del proceso
            // buscarlo en la lista de procesos creados
            // devolverle su contexto de ejecución.
            t_buffer *buffer = serializar_registro(registro);
            log_info(log, "Mandando contexto de ejecución...");
            send(*socket_cpu_mem, buffer, buffer->size, 0);
            eliminar_paquete(paquete);
            buffer_destroy(buffer);
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
}
