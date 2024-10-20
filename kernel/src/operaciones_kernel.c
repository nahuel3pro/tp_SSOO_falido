#include <../include/operaciones_kernel.h>

bool _has_less_priority(void *a, void *b)
{
    t_TCB thread_a = (t_TCB )a;
    t_TCB thread_b = (t_TCB )b;

    return thread_a->priority < thread_b->priority;
}

void planificar_corto_plazo(void)
{
    pthread_t hilo_corto_plazo;
    pthread_create(&hilo_corto_plazo, NULL, (void *)exec_tcb, NULL);
    pthread_detach(hilo_corto_plazo);
}

void planificar_largo_plazo(void)
{
    pthread_t hilo_ready;
    pthread_t hilo_exit;
    pthread_create(&hilo_ready, NULL, (void *)ready_tcb, NULL);
    pthread_create(&hilo_exit, NULL, (void *)exit_tcb, NULL);
    pthread_detach(hilo_exit);
    pthread_detach(hilo_ready);
}

void exit_tcb(void)
{
    while (1)
    {
        sem_wait(&sem_exit);
        t_TCB tcb_to_remove = safe_tcb_remove(exit_queue, &mutex_cola_exit);
        // Fijarse si tenía un hilo esperándolo, y devolverlo a ready
        if (tcb_to_remove->TID_wait > -1)
        {
            thread_back_to_ready(tcb_to_remove->PID, tcb_to_remove->TID_wait);
        }
        // mandar a memoria TID a eliminar.
        send_to_mem_thread_kill(tcb_to_remove->PID, tcb_to_remove->TID_wait);

        free(tcb_to_remove);
        tcb_to_remove = NULL;
    }
}

void ready_tcb(void) // Procesos de la cola NEW para mandarlos a READY
{
    while (1)
    {
        sem_wait(&sem_new); // cola new
        t_PCB pcb_aux = safe_pcb_remove(new_queue, &mutex_cola_new);
        t_TCB tcb_aux = list_get(pcb_aux->TIDs, 0);
        // sem_post(&sem_ready);
        process_create(tcb_aux->file_path, pcb_aux->size, tcb_aux->priority);
    }
}

void exec_tcb()
{
    int fd = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
    while (1)
    {
        sem_wait(&sem_ready);
        sem_wait(&sem_exec);
        log_trace(log, "Eligiendo TCB...");
        t_TCB tcb = elegir_tcb_segun_algoritmo();
        // MANDAR A CPU EL TCB A EJECUTAR.
        dispatch(tcb, fd);
        t_buffer *buffer_response = malloc(sizeof(t_buffer));
        buffer_recv(fd, buffer_response);
        int str_size;
        char *motivo = buffer_read_string(buffer_response, str_size);
        atender_motivo(motivo, buffer_response);
    }
}

t_TCB elegir_tcb_segun_algoritmo()
{
    switch (ALGORITMO_PLANIFICACION)
    {
    case FIFO:
        return safe_tcb_remove(ready_list, &mutex_cola_ready);
    case PRIORIDADES:
        return safe_tcb_remove(list_sorted(ready_list, _has_less_priority), &mutex_cola_ready);
        break;
    case CMN:
        log_info(log, "CMN");
        break;
    default:
        log_error(log, "No se reconoció el algoritmo de planifación");
        abort();
    }
    return NULL;
}

void safe_tcb_add(t_list *list, t_TCB tcb, pthread_mutex_t *mutex)
{
    pthread_mutex_lock(mutex);
    list_add(list, tcb);
    pthread_mutex_unlock(mutex);
}

t_TCB safe_tcb_remove(t_list *list, pthread_mutex_t *mutex)
{
    t_TCB tcb;
    pthread_mutex_lock(mutex);
    tcb = list_remove(list, 0);
    pthread_mutex_unlock(mutex);
    return tcb;
}

void safe_pcb_add(t_queue *queue, t_PCB pcb, pthread_mutex_t *mutex)
{
    pthread_mutex_lock(mutex);
    queue_push(queue, pcb);
    pthread_mutex_unlock(mutex);
}

t_PCB safe_pcb_remove(t_queue *queue, pthread_mutex_t *mutex)
{
    t_PCB pcb;
    pthread_mutex_lock(mutex);
    pcb = queue_pop(queue);
    pthread_mutex_unlock(mutex);
    return pcb;
}

void send_to_mem_process_kill(int pid)
{
    // Conectarse a memoria
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL);

    t_paquete *paquete_send = crear_paquete(PROCESS_KILL);
    t_buffer *buffer_send = buffer_create(SIZEOF_UINT32);
    buffer_add_uint32(buffer_send, (uint32_t)pid);

    buffer_send->offset = 0;

    paquete_send->buffer = buffer_send;
    enviar_paquete(paquete_send, socket_cliente);
    eliminar_paquete(paquete_send);

    uint8_t confirmation;
    recv(socket_cliente, &confirmation, SIZEOF_UINT8, 0);
    close(socket_cliente);
}

void send_to_mem_thread_kill(int PID, int TID)
{
    // Conectarse a memoria
    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL);

    t_paquete *paquete_send = crear_paquete(PROCESS_KILL);
    t_buffer *buffer_send = buffer_create(SIZEOF_UINT32);
    buffer_add_uint32(buffer_send, (uint32_t)PID);
    buffer_add_uint32(buffer_send, (uint32_t)TID);

    buffer_send->offset = 0;

    paquete_send->buffer = buffer_send;
    enviar_paquete(paquete_send, socket_cliente);
    eliminar_paquete(paquete_send);
    close(socket_cliente);
}

void send_pid_exit(int PID)
{
    void exit_process(void *ptr)
    {
        t_TCB tcb_aux = (t_TCB)ptr;
        // sacarlos de la lista de ready y mandarlos a exit
        sem_wait(&sem_ready);
        list_remove_element(ready_list, tcb_aux);
        safe_tcb_add(exit_queue, tcb_aux, &mutex_cola_exit);
        sem_post(&sem_exit);
    }
    t_PCB pcb_to_remove = get_process(PID);
    list_iterate(pcb_to_remove->TIDs, exit_process);
}

void send_tid_exit()
{
    t_TCB tcb_to_exit = safe_tcb_remove(ready_list, &mutex_cola_ready);
    safe_tcb_add(exit_queue, tcb_to_exit, &mutex_cola_exit);
    sem_post(&sem_exit);
}

void mutex_create(int pid, char *mutex_name)
{
    log_info(log, "Creando mutex en proceso: %d y con nombre %s", pid, mutex_name);
}