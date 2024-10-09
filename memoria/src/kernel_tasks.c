#include "../include/kernel_tasks.h"

void atenderKernel(void *void_args)
{
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    int *socket_kernel_mem = args->fd;
    free(args);

    log_info(log, "## Kernel Conectado - FD del socket: <%d>", *socket_kernel_mem);
    uint8_t res = SUCCESS;
    // Primero recibimos el codigo de operacion
    uint8_t op_code;
    recv(*socket_kernel_mem, &(op_code), SIZEOF_UINT8, MSG_WAITALL);
    switch (op_code)
    {
    case PROCESS_CREATION:
        log_info(log, "Creando proceso...");
        process_create(*socket_kernel_mem);
        break;
    case PROCESS_KILL:
        log_info(log, "Matando el proceso");
        send(*socket_kernel_mem, &res, SIZEOF_UINT8, 0);
        break;
    case THREAD_CREATION:
        log_info(log, "Creando thread...");
        send(*socket_kernel_mem, &res, SIZEOF_UINT8, 0);
        break;
    case MEMORY_DUMP:
        log_info(log, "Dumpeando...");
        send(*socket_kernel_mem, &res, SIZEOF_UINT8, 0);
        break;
    default:
        log_warning(log, "Operacion desconocida. No quieras meter la pata");
        break;
    }
    // Se cierra la conexión con el kernel
    close(*socket_kernel_mem);
    free(socket_kernel_mem);
}

void load_list_instructions(t_list *list_instructions, char *path)
{
    char *line;

    for (int i = 0; (line = get_next_line(path)) != NULL; i++)
    {
        char *add = malloc(string_length(line) + 1);
        memcpy(add, line, string_length(line) + 1);
        list_add(list_instructions, add);
    }
}

void initiate_registers(t_register *registro)
{
    registro->PC = 0;
    registro->AX = 0;
    registro->BX = 0;
    registro->CX = 0;
    registro->DX = 0;
    registro->EX = 0;
    registro->FX = 0;
    registro->GX = 0;
    registro->HX = 0;
    registro->base = 0;
    registro->limite = 0;
}

void process_create(int socket_kernel_mem)
{
    // Hacer lista de procesos, e inicializar correctamente el proceso que llega de kernel.
    t_paquete *paquete = malloc(sizeof(t_paquete));
    crear_buffer(paquete);

    recv(socket_kernel_mem, &(paquete->buffer->size), SIZEOF_UINT32, 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket_kernel_mem, paquete->buffer->stream, paquete->buffer->size, 0);
    // Des-serealizando ---> Crear función. y liberar la memoria a quien corresponda.
    uint32_t pid = buffer_read_uint32(paquete->buffer);
    uint32_t size = buffer_read_uint32(paquete->buffer);
    uint32_t str_size;
    char *path_file = buffer_read_string(paquete->buffer, &str_size);
    uint32_t priority = buffer_read_uint32(paquete->buffer);
    // cargar PCB recibido e inicializarlo con el TCB principal.
    t_PCB pcb = malloc(sizeof(t_PCB));
    pcb->PID = pid;
    pcb->size = size;
    pcb->TIDs = list_create();
    t_TCB tcb = malloc(sizeof(t_TCB));
    tcb->TID = 0;
    tcb->PID = pid;
    tcb->priority = priority;
    tcb->instructions = list_create();
    //load_list_instructions(tcb->instructions, path_file); // Acá hay un problema de memoria o threads
    initiate_registers(&(tcb->registers)); // Acá hay un problema, por algún motivo
    log_info(log, "## Proceso <Creado> -  PID: <%d> - Tamaño: <%d>", pid, size);
    list_add(process_list, pcb);
    list_add(pcb->TIDs, tcb);

    // Demorar tiempo y responder
    int wait_time = config_get_int_value(config, "RETARDO_RESPUESTA");
    usleep(wait_time); // Espera de un segundo

    uint8_t res = SUCCESS;
    send(socket_kernel_mem, &res, SIZEOF_UINT8, 0);
    eliminar_paquete(paquete);
}