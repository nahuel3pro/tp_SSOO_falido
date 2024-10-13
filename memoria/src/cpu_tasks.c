#include "../include/cpu_tasks.h"

void atenderCpu(void *void_args)
{
    // log_info(log, "atendiendo cpu con memoria...");
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    // t_log *logger = args->log;
    int *socket_cpu_mem = args->fd;
    free(args);
    log_info(log, "## CPU Conectado - FD del socket: <%d>", *socket_cpu_mem);
    uint8_t op_code;
    while (1)
    {

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
            update_context(*socket_cpu_mem);
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
    }
    close(*socket_cpu_mem);
    free(socket_cpu_mem);
    // free(args);
}

void execution_context(int socket)
{
    // recibir PID, TID:
    t_paquete *paquete_recv = malloc(sizeof(t_paquete));
    crear_buffer(paquete_recv);
    recv(socket, &paquete_recv->buffer->size, SIZEOF_UINT32, 0);
    paquete_recv->buffer->stream = malloc(paquete_recv->buffer->size);
    recv(socket, paquete_recv->buffer->stream, paquete_recv->buffer->size, 0);

    uint32_t PID = buffer_read_uint32(paquete_recv->buffer);
    uint32_t TID = buffer_read_uint32(paquete_recv->buffer);
    eliminar_paquete(paquete_recv);
    t_TCB thread_aux = get_thread(PID, TID); //--problema con registros
    log_info(log, "## Contexto <Solicitado> - (PID:TID) - (<%d:%d>)", PID, TID);
    t_buffer *buffer_registro = buffer_create(sizeof(t_register));
    serializar_registro(buffer_registro, thread_aux->registers);
    buffer_registro->offset = 0;
    retardo_respuesta();
    enviar_buffer(buffer_registro, socket);
    log_info(log, "Mandando contexto de ejecución...");
    buffer_destroy(buffer_registro);
}

void get_instruction(int socket)
{
    // // CPU NOS MANDA, PID, TID, PC.
    t_paquete *paquete_recv = malloc(sizeof(t_paquete));
    crear_buffer(paquete_recv);
    recv(socket, &paquete_recv->buffer->size, SIZEOF_UINT32, 0);
    paquete_recv->buffer->stream = malloc(paquete_recv->buffer->size);
    recv(socket, paquete_recv->buffer->stream, paquete_recv->buffer->size, 0);

    uint32_t PID = buffer_read_uint32(paquete_recv->buffer);
    uint32_t TID = buffer_read_uint32(paquete_recv->buffer);
    uint32_t PC = buffer_read_uint32(paquete_recv->buffer);

    eliminar_paquete(paquete_recv);

    t_TCB thread_aux = get_thread(PID,TID);
    char *instruction = list_get(thread_aux->instructions, PC);
    //                                        Tamaño PL + tamaño del string  + "\0"
    t_buffer *buffer_send = buffer_create(SIZEOF_UINT32 + strlen(instruction) + 1);

    buffer_add_string(buffer_send, instruction);
    buffer_send->offset = 0;

    retardo_respuesta();

    enviar_buffer(buffer_send, socket);

    buffer_destroy(buffer_send);

    // log_info(log, "## Obtener instrucción - (PID:TID) - (<%d>:<%d>) - Instrucción: <INSTRUCCIÓN> <...ARGS>");
}

void update_context(int socket_cpu_mem)
{
    t_paquete *paquete_recv = malloc(sizeof(t_paquete));
    crear_buffer(paquete_recv);
    recv(socket, &paquete_recv->buffer->size, SIZEOF_UINT32, 0);
    paquete_recv->buffer->stream = malloc(paquete_recv->buffer->size);
    recv(socket, paquete_recv->buffer->stream, paquete_recv->buffer->size, 0);
    // PID - TID - REGISTROS
    uint32_t PID = buffer_read_uint32(paquete_recv->buffer);
    uint32_t TID = buffer_read_uint32(paquete_recv->buffer);
    // Deserealizar registro que me mandó CPU
    t_register registros;
    deserealizar_registro(paquete_recv->buffer, &registros);

    // Buscar PID y TID en la lista de registros, una vez encontrado,
    // actualizar sus registros.

    // mandar respuesta correcta a CPU?
}
