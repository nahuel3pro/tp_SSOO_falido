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
        thread_create(*socket_kernel_mem);
        break;
    case THREAD_KILL:
        log_trace(log, "Matando un hilo...");
        thread_cancel(*socket_kernel_mem);
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

void process_create(int socket_kernel_mem)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    crear_buffer(paquete);

    recv(socket_kernel_mem, &(paquete->buffer->size), SIZEOF_UINT32, 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket_kernel_mem, paquete->buffer->stream, paquete->buffer->size, 0);

    uint32_t pid = buffer_read_uint32(paquete->buffer);
    uint32_t size = buffer_read_uint32(paquete->buffer);
    uint32_t priority = buffer_read_uint32(paquete->buffer);
    uint32_t str_size;
    const char *path_file = buffer_read_string(paquete->buffer, &str_size);
    eliminar_paquete(paquete);
    // cargar PCB recibido e inicializarlo con el TCB principal.
    t_PCB pcb = process_initiate(pid, size);
    t_TCB tcb = thread_initiate(path_file, priority, pid, 0); // TID 0 por ser TCB principal.
    safe_pcb_add(process_list, pcb, &mutex_process_list);
    list_add(pcb->TIDs, tcb);

    // Demorar tiempo y responder
    retardo_respuesta();
    log_info(log, "## Proceso <Creado> -  PID: <%d> - Tamaño: <%d>", pid, size); // log obligatorio
    
    uint8_t res = SUCCESS;
    send(socket_kernel_mem, &res, SIZEOF_UINT8, 0);
}

void thread_create(int socket_kernel_mem)
{
    uint32_t buffer_size;
    recv(socket_kernel_mem, &buffer_size, SIZEOF_UINT32, 0);
    t_buffer *buffer_rcv = buffer_create(buffer_size);
    recv(socket_kernel_mem, buffer_rcv->stream, buffer_size, 0);

    int PID = buffer_read_uint32(buffer_rcv);
    int thread_priority = buffer_read_uint32(buffer_rcv);
    int str_size;
    char *file_path = buffer_read_string(buffer_rcv, &str_size);
    buffer_destroy(buffer_rcv);
    // Buscar PID para añadirle el thread
    t_PCB pcb_aux = get_process(PID);
    int thread_qnty = list_size(pcb_aux->TIDs);
    t_TCB new_thread = thread_initiate(file_path, thread_priority, PID, thread_qnty);
    thread_initiate(file_path, thread_priority, pcb_aux->PID, thread_qnty);
    list_add(pcb_aux->TIDs, new_thread);
    log_info(log, "## Hilo <Creado> - (PID:TID) - (<%d>:<%d>)", PID, thread_qnty); // Log obligatorio
}

void thread_cancel(int socket_kernel_mem){
    t_paquete *paquete = malloc(sizeof(t_paquete));
    crear_buffer(paquete);

    recv(socket_kernel_mem, &(paquete->buffer->size), SIZEOF_UINT32, 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket_kernel_mem, paquete->buffer->stream, paquete->buffer->size, 0);

    uint32_t PID = buffer_read_uint32(paquete->buffer);
    uint32_t TID = buffer_read_uint32(paquete->buffer);
    eliminar_paquete(paquete);

    t_TCB tcb_to_kill = take_thread(PID, TID);
    
    free_tcb(tcb_to_kill);
}