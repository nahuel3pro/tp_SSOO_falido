#include "../include/operaciones_memoria.h"

// Recive el pedido de instrucción hecho anteriormente.
char *recv_instruction(int socket_cliente, uint32_t PID, uint32_t TID, uint32_t PC)
{
    log_info(log, "## TID: <%d> - FETCH - Program Counter: <%d>", TID, PC); // Log obligatorio
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

    buffer_destroy(buffer_r);
    return inst;
}

char *fetch(int socket_cliente, t_register *registro, uint32_t PID, uint32_t TID)
{

    get_context(PID, TID, registro, socket_cliente);

    return recv_instruction(socket_cliente, PID, TID, registro->PC);
}

void decode_execute(char *instruction, t_register *registro, uint32_t PID, uint32_t TID, int client_dispatch_fd)
{

    char **array = string_split(instruction, " ");
    t_dictionary *dic = dict_protocol();
    uint32_t valor1;
    uint32_t valor2;
    int dic_instruction = dictionary_get(dic, array[0]);
    switch (dic_instruction)
    {
    case INSTRUCCION_SET:
        log_info(log, "## TID: <%d> - Ejecutando: <%s> - <%s, %d>", TID, array[0], array[1], atoi(array[2]));
        set_registro(registro, array[1], atoi(array[2]));
        break;

    case INSTRUCCION_READ_MEM:
        log_info(log, "## TID: <%d> - Ejecutando: <%s> - <%s, %d>", TID, array[0], array[1], atoi(array[2]));

        valor1 = obtener_registro(array[2], registro);

        set_registro(registro, array[1], valor1);
        break;

    case INSTRUCCION_WRITE_MEM:
        // TODO
        break;

    case INSTRUCCION_SUM:
        log_info(log, "## TID: <%d> - Ejecutando: <%s> - <%s, %d>", TID, array[0], array[1], atoi(array[2]));

        valor1 = obtener_registro(array[1], registro);
        valor2 = obtener_registro(array[2], registro);

        set_registro(registro, array[1], valor1 + valor2);
        break;

    case INSTRUCCION_SUB:
        log_info(log, "## TID: <%d> - Ejecutando: <%s> - <%s, %s>", TID, array[0], array[1], array[2]);

        valor1 = obtener_registro(array[1], registro);
        valor2 = obtener_registro(array[2], registro);

        set_registro(registro, array[1], valor1 - valor2);
        break;

    case INSTRUCCION_JNZ:
        log_info(log, "## TID: <%d> - Ejecutando: <%s> - <%s, %d>", TID, array[0], array[1], atoi(array[2]));

        valor1 = obtener_registro(array[1], registro);

        if (valor1 != 0)
        {
            set_registro(registro, "PC", atoi(array[2]));
            break;
        }
        break;

    case INSTRUCCION_LOG:
        log_info(log, "## TID: <%d> - Ejecutando: <%s> - <%s>", TID, array[0], array[1]);

        valor1 = obtener_registro(array[1], registro);
        log_info(log, valor1);
        break;

    // ----- SYSCALLS ----- MANDAR MOTIVO A KERNEL
    case INSTRUCCION_DUMP_MEMORY:
        send_syscall(array[0], "DUMP_MEMORY", client_dispatch_fd); // No requiere parámetros adicionales
        break;
    case INSTRUCCION_MUTEX_CREATE:
        send_syscall(array[0], "MUTEX_CREATE", client_dispatch_fd, recurso1); // Crear el mutex del recurso1
        break;

    case INSTRUCCION_MUTEX_UNLOCK:
         send_syscall(array[0], "MUTEX_UNLOCK", client_dispatch_fd, recurso1); // Liberar el mutex del recurso1
        break;
    case INSTRUCCION_PROCESS_EXIT:
        send_syscall(array[0], "PROCESS_EXIT", client_dispatch_fd); // No necesita parámetros adicionales
        break;
    case INSTRUCCION_PROCESS_CREATE:
        send_syscall(array[0], "PROCESS_CREATE", client_dispatch_fd, "proceso1", 256, 1); // Nombre del proceso (proceso1), tamaño de memoria (256), prioridad (1)
        break;
    case INSTRUCCION_THREAD_CANCEL:
        send_syscall(array[0], "THREAD_CANCEL", client_dispatch_fd, array[1]); // array[1] contiene el TID (en este caso 1)
        break;
    case INSTRUCCION_THREAD_CREATE:
        send_syscall(array[0], "THREAD_CREATE", client_dispatch_fd, "hilo1", 3); // Nombre del hilo (hilo1), prioridad (3)
        break;
    case INSTRUCCION_THREAD_EXIT:
        send_syscall(array[0], 1, client_dispatch_fd);
        break;
    case INSTRUCCION_THREAD_JOIN:
        send_syscall(array[0], "THREAD_JOIN", client_dispatch_fd, array[1]); // array[1] contiene el TID (en este caso 1)
        break;
    default:
        break;
    }
    registro->PC++;
    dictionary_destroy(dic);
}

void update_context(int fd, t_register registro, uint32_t pid, uint32_t tid)
{
    log_info(log, "## TID: <%d> - Actualizo Contexto Ejecución", tid); // Log obligatorio
    t_paquete *paquete = crear_paquete(UPDATE_CONTEXT);
    t_buffer *buffer = buffer_create(SIZEOF_UINT32 * 2 + sizeof(t_register));

    buffer_add_uint32(buffer, pid);
    buffer_add_uint32(buffer, tid);
    serializar_registro(buffer, registro);
    buffer->offset = 0;
    paquete->buffer = buffer;

    enviar_paquete(paquete, fd);
    eliminar_paquete(paquete);

    uint8_t resultado = 0;
    recv(fd, &resultado, SIZEOF_UINT8, 0);

    if (resultado != SUCCESS)
    {
        log_error(log, "Error al actualizar el contexto");
        abort();
    }
}
