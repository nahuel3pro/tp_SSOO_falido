#include "../include/operaciones_memoria.h"

// Recive el pedido de instrucción hecho anteriormente.
char *recv_instruction(int socket_cliente, uint32_t PID, uint32_t TID, uint32_t PC)
{
    // Mando(serializar) TID, PID y PC
    t_paquete *paquete_send_inst = crear_paquete(GET_INSTRUCTION);
    t_buffer *buffer_send_inst = buffer_create(SIZEOF_UINT32 * 3);
    buffer_add_uint32(buffer_send_inst, PID);
    buffer_add_uint32(buffer_send_inst, TID);
    buffer_add_uint32(buffer_send_inst, PC);
    buffer_send_inst->offset = 0;
    paquete_send_inst->buffer = buffer_send_inst;
    enviar_paquete(paquete_send_inst, socket_cliente);
    eliminar_paquete(paquete_send_inst);

    t_buffer *buffer_r;
    buffer_r = malloc(sizeof(t_buffer));
    buffer_recv(socket_cliente, buffer_r);
    // Se recibe el contexto de ejecución.
    uint32_t str_size;
    char *inst = buffer_read_string(buffer_r, &str_size);
    log_trace(log, "Instrucción recibida: %s", inst);

    buffer_destroy(buffer_r);
    return inst;
}

void fetch(int socket_cliente, char *instruction, t_register *registro, uint32_t PID, uint32_t TID)
{

    get_context(PID, TID, registro, socket_cliente);

    instruction = recv_instruction(socket_cliente, PID, TID, registro->PC);
}

void decode_execute(char *instruction, t_register *registro, uint32_t PID, uint32_t TID)
{

    char **array = string_split(instruction, " ");
    log_info(log, "Decodificando");
    t_dictionary *dic = dict_protocol();
    uint32_t valor1;
    uint32_t valor2;
    int dic_instruction = dictionary_get(dic, array[0]);
    switch (dic_instruction)
    {
    case INSTRUCCION_SET:
        set_registro(registro, array[1], atoi(array[2]));
        break;

    case INSTRUCCION_READ_MEM:
        valor1 = obtener_registro(array[2], registro);

        set_registro(registro, array[1], valor1);
        break;

    case INSTRUCCION_WRITE_MEM:
        // TODO
        break;

    case INSTRUCCION_SUM:
        valor1 = obtener_registro(array[1], registro);
        valor2 = obtener_registro(array[2], registro);

        set_registro(registro, array[1], valor1 + valor2);
        break;

    case INSTRUCCION_SUB:
        valor1 = obtener_registro(array[1], registro);
        valor2 = obtener_registro(array[2], registro);

        set_registro(registro, array[1], valor1 - valor2);
        break;

    case INSTRUCCION_JNZ:
        valor1 = obtener_registro(array[1], registro);
        valor2 = obtener_registro(array[2], registro);

        if (valor1 != 0)
        {
            set_registro(registro, "PC", valor2 - 1);
            break;
        }
        break;

    case INSTRUCCION_LOG:
        valor1 = obtener_registro(array[1], registro);
        log_info(log, valor1);
        break;

    // ----- SYSCALLS -----
    case INSTRUCCION_DUMP_MEMORY:
        log_info(log, "DUMP MEMORY");
        break;
    case INSTRUCCION_MUTEX_CREATE:
        log_info(log, "MUTEX_CREATE");
        break;
    case INSTRUCCION_MUTEX_UNLOCK:
        log_info(log, "INSTRUCCION_MUTEX_UNLOCK");
        break;
    case INSTRUCCION_PROCESS_EXIT:
        log_info(log, "INSTRUCCION_PROCESS_EXIT");
        break;
    case INSTRUCCION_PROCESS_CREATE:
        log_info(log, "INSTRUCCION_PROCESS_CREATE");
        break;
    case INSTRUCCION_THREAD_CANCEL:
        log_info(log, "INSTRUCCION_THREAD_CANCEL");
        break;
    case INSTRUCCION_THREAD_CREATE:
        log_info(log, "INSTRUCCION_THREAD_CREATE");
        break;
    case INSTRUCCION_THREAD_EXIT:
        log_info(log, "INSTRUCCION_THREAD_EXIT");
        break;
    case INSTRUCCION_THREAD_JOIN:
        log_info(log, "INSTRUCCION_THREAD_JOIN");
        break;
    default:
        break;
    }
    registro->PC++;
    dictionary_destroy(dic);
}