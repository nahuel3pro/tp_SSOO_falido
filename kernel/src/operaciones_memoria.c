#include "../include/operaciones_memoria.h"

void process_create(char *filename, int process_size, int thread_priority)
{
    t_PCB new_process = malloc(sizeof(t_PCB));
    new_process->PID = rand(); // Generar un PID único
    new_process->TIDs = list_create();
    new_process->mutex = list_create();

    t_TCB main_thread = malloc(sizeof(t_TCB));
    main_thread->PID = new_process->PID;
    main_thread->TID = 0; // El hilo principal siempre tiene TID 0
    main_thread->priority = thread_priority;
    list_add(new_process->TIDs, main_thread);

    log_info(log, "Creación de Proceso: ## (<PID>:%d) Se crea el proceso - Estado: NEW", new_process->PID);

    // avisarle a memoria y esperar confirmación.
    // mandar PID, process_size y file_name
    // Conectarse a memoria
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL);

    t_paquete *paquete_send = crear_paquete(PROCESS_CREATION);
    t_buffer *buffer_send = buffer_create(SIZEOF_UINT32 * 3 + strlen(filename) + 1);
    buffer_add_uint32(buffer_send, (uint32_t)new_process->PID);
    buffer_add_uint32(buffer_send, (uint32_t)process_size);
    buffer_add_string(buffer_send, filename);
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
    }
    else
    {
        // Se manda a la cola NEW para esperar a ser inicializado nuevamente.
        queue_push(new_queue, new_process);
    }

    log_info(log, "Proceso creado y enviado a la cola NEW");
}

/* void thread_create(int pid, int priority, char *file_path)
{
    t_PCB process = find_process_by_pid(pid);

    if (process == NULL)
    {
        log_info(log, "Error: Proceso no encontrado");
        return;
    }

    // Creamos un nuevo hilo
    t_TCB new_thread = malloc(sizeof(t_TCB));
    new_thread->TID = rand(); // Generar un TID único
    new_thread->priority = priority;

    list_add(process->TIDs, new_thread);
    // Avisarle a memoria y esperar confirmación.
    // mandar el file_path y PID

    // Enviar el hilo a la cola READY
    queue_push(ready_queue, new_thread);

    log_info(log, "Hilo creado y enviado a la cola READY");
} */