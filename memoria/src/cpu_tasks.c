#include "../include/cpu_tasks.h"

void atenderCpu(void *void_args)
{
    // log_info(log, "atendiendo cpu con memoria...");
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    // t_log *logger = args->log;
    int *socket_cpu_mem = args->fd;
    free(args);

    uint8_t op_code;
    recv(*socket_cpu_mem, &(op_code), SIZEOF_UINT8, MSG_WAITALL);
    switch (op_code)
    {
    case GET_EXECUTION_CONTEXT:
        // recibir el pid del proceso
        // buscarlo en la lista de procesos creados
        // devolverle su contexto de ejecución.
        execution_context(*socket_cpu_mem);
        break;
    case UPDATE_CONTEXT:
        log_info(log, "Actualizando contexto de ejecución...");
        break;
    case GET_INSTRUCTION:
        log_info(log, "Dándole la instrucción...");
        get_instruction(*socket_cpu_mem);
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
    // eliminar_paquete(paquete);
    close(socket_cpu_mem);
    free(socket_cpu_mem);
    // free(args);
}

void execution_context(int socket)
{
    // CPU pide este registro
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

    uint32_t PID;
    log_info(log, "## Contexto <Solicitado> - (PID:TID) - (<%d:<TID>)", PID);
    t_buffer *buffer = buffer_create(sizeof(t_register));
    serializar_registro(buffer, registro);
    buffer->offset = 0;
    wait();
    send_data(69, buffer, socket);
    log_info(log, "Mandando contexto de ejecución...");
    // buffer_destroy(buffer);
}

void get_instruction(int socket)
{
    // // CPU NOS MANDA, SOLO MANDAR ESTO.
    // int pib = 1;
    // int tid = 0;
    // int pc = 3;

    // // Obtener instrucción de PCB y TID
    // t_PCB pcb_buffer = malloc(sizeof(t_PCB));
    // pcb_buffer = list_get(process_list, pib);
    t_TCB tcb_buffer = malloc(sizeof(t_TCB));
    // tcb_buffer = list_get(pcb_buffer->TIDs,tid);
    // // obtener instrucción del TID
    // char* instrution = list_get(tcb_buffer->instructions, pc);

    // Serializar instrucción
    char *path = "/home/utnso/tp-2024-2c-La-Daneta/kernel/test_psdc/test2.dat";
    // obtener instrucción de PC
    char *instrution = "SET AX 1";
                                        // Tamaño PL + tamaño del string  + "\0" 
    t_buffer *buffer1 = buffer_create( SIZEOF_UINT32 + strlen(instrution) + 1);

    buffer_add_string(buffer1, instrution);

    buffer1->offset = 0;

    wait();
    send_data(69, buffer1, socket);

    //log_info(log, "## Obtener instrucción - (PID:TID) - (<%d>:<%d>) - Instrucción: <INSTRUCCIÓN> <...ARGS>");
}