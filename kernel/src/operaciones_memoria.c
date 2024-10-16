#include "../include/operaciones_memoria.h"

void process_create(char *filename, int process_size, int main_thread_priority)
{
    t_PCB new_process = malloc(sizeof(t_PCB));
    new_process->PID = rand(); // Generar un PID único
    new_process->TIDs = list_create();
    new_process->mutex = list_create();

    t_TCB main_thread = malloc(sizeof(t_TCB));
    main_thread->PID = new_process->PID;
    main_thread->TID = 0; // El hilo principal siempre tiene TID 0
    main_thread->priority = main_thread_priority;
    main_thread->TID_wait = -1;
    list_add(new_process->TIDs, main_thread);

    log_info(log, "Creación de Proceso: ## (<PID>:%d) Se crea el proceso - Estado: NEW", new_process->PID);

    // avisarle a memoria y esperar confirmación.
    // mandar PID, process_size y file_name
    // Conectarse a memoria
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL);

    t_paquete *paquete_send = crear_paquete(PROCESS_CREATION);
    t_buffer *buffer_send = buffer_create(SIZEOF_UINT32 * 4 + strlen(filename) + 1);
    buffer_add_uint32(buffer_send, (uint32_t)new_process->PID);
    buffer_add_uint32(buffer_send, (uint32_t)process_size);
    buffer_add_string(buffer_send, filename);
    buffer_add_uint32(buffer_send, (uint32_t)main_thread_priority);
    buffer_send->offset = 0;

    paquete_send->buffer = buffer_send;
    enviar_paquete(paquete_send, socket_cliente);
    eliminar_paquete(paquete_send);

    // Se espera la respuesta positiva de memoria para poder mandarlo a la cola de READY.
    int res = 0;
    recv(socket_cliente, &res, SIZEOF_UINT32, MSG_WAITALL);

    // Acá va a haber que usar MUTEX para las COLAS.
    if (res == SUCCESS)
    {
        // Enviar el proceso a la cola READY.
        list_add(ready_list, main_thread);
        log_trace(log, "Proceso creado exitosamente y enviado a la cola READY!");
    }
    else
    {
        // Se manda a la cola NEW para esperar a ser inicializado nuevamente.
        queue_push(new_queue, new_process);
        log_trace(log, "Proceso no pudo ser creado y se envió a la cola NEW");
    }
    close(socket_cliente);
}

void thread_create(char *filename, int pid, int main_thread_priority)
{
    //Necesitamos obtener el PCB en la lista de pcbs
    t_PCB process = remover_por_PID(blocked_queue, pid);
    
    int tid_to_set = list_size(&process->TIDs);

    t_TCB main_thread = malloc(sizeof(t_TCB));
    main_thread->PID = process->PID;
    main_thread->TID = tid_to_set; // El tid tiene que ser autoincremental, entonces necesitamos la lista de tids del proceso
    main_thread->priority = main_thread_priority;
    main_thread->TID_wait = -1;
    list_add(process->TIDs, main_thread);

    // Conectarse a memoria
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL);

    t_paquete *paquete_send = crear_paquete(PROCESS_CREATION);
    t_buffer *buffer_send = buffer_create(SIZEOF_UINT32 * 4 + strlen(filename) + 1);
    buffer_add_uint32(buffer_send, (uint32_t)process->PID);
    buffer_add_uint32(buffer_send, (uint32_t)process->size);
    buffer_add_string(buffer_send, filename);
    buffer_add_uint32(buffer_send, (uint32_t)main_thread_priority);
    buffer_send->offset = 0;

    paquete_send->buffer = buffer_send;
    enviar_paquete(paquete_send, socket_cliente);
    eliminar_paquete(paquete_send);

    // Se espera la respuesta positiva de memoria para poder mandarlo a la cola de READY.
    int res = 0;
    recv(socket_cliente, &res, SIZEOF_UINT32, MSG_WAITALL);

    // Acá va a haber que usar MUTEX para las COLAS.
    if (res == SUCCESS)
    {
        // Enviar el proceso a la cola READY.
        list_add(ready_list, main_thread);
        log_trace(log, "Proceso creado exitosamente y enviado a la cola READY!");
    }
}
