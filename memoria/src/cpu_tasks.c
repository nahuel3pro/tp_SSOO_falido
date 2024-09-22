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
        // // CPU NOS MANDA, SOLO MANDAR ESTO.
        // int pib = 1;
        // int tid = 0;
        // int pc = 3;

        // // Obtener registro de PCB y TID
        // t_PCB pcb_buffer = malloc(sizeof(t_PCB));
        // pcb_buffer = list_get(process_list, pib);
        // t_TCB tcb_buffer = malloc(sizeof(t_TCB));
        // tcb_buffer = list_get(pcb_buffer->TIDs,tid);

        // // obtener instrucción de PC
        // char* instrution = list_get(tcb_buffer->instructions, pc);

        // Serializar contexto (registros) e instrucción
        char* path = "/home/utnso/tp-2024-2c-La-Daneta/kernel/test_psdc/test2.dat";
        t_register* registro_ = initiate_registers();
        // obtener instrucción de PC
        char* instrution = "SET AX 1";

        
        t_buffer *buffer1 = serializar_registro(*registro_);
        //uint32_t a = buffer_read_uint32(buffer1);
        buffer_add_string(buffer1, instrution);
        buffer1->offset = 0;

        send_data(69, buffer1, *socket_cpu_mem);

        log_info(log, "## Obtener instrucción - (PID:TID) - (<%d>:<%d>) - Instrucción: <INSTRUCCIÓN> <...ARGS>");
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
