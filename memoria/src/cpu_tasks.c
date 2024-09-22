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
    registro.DX = 222;
    registro.EX = 99;
    registro.FX = 230;
    registro.GX = 666;
    registro.HX = 332;
    registro.base = 24;
    registro.limite = 250000;

    t_paquete *paquete = malloc(sizeof(t_paquete));
    crear_buffer(paquete);
    recv(*socket_cpu_mem, &(paquete->op_code), SIZEOF_UINT8, MSG_WAITALL);
    switch (paquete->op_code)
    {
    case GET_EXECUTION_CONTEXT:
        // recibir el pid del proceso
        // buscarlo en la lista de procesos creados
        // devolverle su contexto de ejecución.
        uint32_t PID;
        log_info(log, "## Contexto <Solicitado> - (PID:TID) - (<%d:<TID>)", PID);
        t_buffer *buffer = serializar_registro(registro);
        wait();
        send_data(69, buffer, *socket_cpu_mem);
        log_info(log, "Mandando contexto de ejecución...");
        eliminar_paquete(paquete);
        buffer_destroy(buffer);
        break;
    case UPDATE_CONTEXT:
        log_info(log, "Actualizando contexto de ejecución...");
        break;
    case GET_INSTRUCTION:
        int last_line_read;
        char *path = "/home/utnso/Desktop/tp-2024-2c-La-Daneta/kernel/test_psdc/test2.dat";
        log_info(log, "Obteniendo instrucción de ejecución...");
        char *line;
        line = get_next_line(path, &last_line_read);
        log_info(log, line);
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
    close(socket_cpu_mem);
    free(socket_cpu_mem);
}
