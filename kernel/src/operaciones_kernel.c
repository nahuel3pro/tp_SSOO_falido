#include <../include/operaciones_kernel.h>

bool _has_less_priority(void *a, void *b)
{
    t_TCB thread_a = (t_TCB *)a;
    t_TCB thread_b = (t_TCB *)b;

    return thread_a->priority < thread_b->priority;
}

void planificar_corto_plazo()
{
    pthread_t hilo_corto_plazo;
    pthread_create(&hilo_corto_plazo, NULL, (void *)exec_tcb, NULL);
    pthread_detach(hilo_corto_plazo);
}

void planificar_largo_plazo()
{
    pthread_t hilo_ready;
    pthread_t hilo_exit;
    pthread_t hilo_block;
    pthread_create(&hilo_ready, NULL, (void *)ready_tcb, NULL);
    pthread_create(&hilo_exit, NULL, (void *)exit_tcb, NULL);
    pthread_create(&hilo_block, NULL, (void *)block_return_tcb, NULL);
    pthread_detach(hilo_exit);
    pthread_detach(hilo_ready);
    pthread_detach(hilo_block);
}

void exit_tcb(void)
{
    while (1)
    {
        sem_wait(&sem_exit);
        t_TCB tcb = safe_tcb_remove(exit_queue, &mutex_cola_exit);
        // mandar a memoria TID a eliminar.
        free(tcb);
    }
}

void ready_tcb(void) // Procesos de la cola NEW para mandarlos a READY
{
    while (1)
    {
        sem_wait(&sem_listos_ready); // cola new
        t_PCB pcb = safe_pcb_remove(new_queue, &mutex_cola_listos_para_ready);
        t_TCB aux = list_get(pcb->TIDs, 0);
        sem_post(&sem_ready);
        process_create(aux->file_path, pcb->size, aux->priority);
    }
}

void block_return_tcb()
{
    while (1)
    {
        sem_wait(&sem_block_return);
        t_TCB tcb = safe_tcb_remove(blocked_queue, &mutex_cola_block);
        // set_tcb_ready(tcb);
        sem_post(&sem_ready);
    }
}

void exec_tcb()
{
    while (1)
    {
        /*  sem_wait(&sem_ready);
         sem_wait(&sem_exec); */
        log_trace(log, "Eligiendo TCB...");
        t_TCB tcb = elegir_tcb_segun_algoritmo();
        // MANDAR A CPU EL TCB A EJECUTAR.
        int fd = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
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
        log_info(log, "Algoritmo de prioridades");
        return safe_tcb_remove(list_sorted(ready_list, _has_less_priority), &mutex_cola_ready);
        break;
    case CMN:
        log_info(log, "CMN");
        break;
    default:
        log_error(log, "No se reconocio el algoritmo de planifacion");
        exit(1);
    }
}

void safe_tcb_add(t_list *list, t_TCB *tcb, pthread_mutex_t *mutex)
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
    list_add(queue, pcb);
    pthread_mutex_unlock(mutex);
}

t_PCB safe_pcb_remove(t_queue *queue, pthread_mutex_t *mutex)
{
    t_PCB pcb;
    pthread_mutex_lock(mutex);
    pcb = list_remove(queue, 0);
    pthread_mutex_unlock(mutex);
    return pcb;
}

void send_pcb_exit(int pid)
{
    // Buscar en cola de procesos y mandar a exit todos sus hilos
    // t_PCB pcb_to_remove = remover_por_PID()

    // Poner proceso en la lista de exit
    // safe_pcb_add()

    // Avisa a memoria y le envia el pid del proceso a eliminar
    send_to_mem(pid);
}

void send_to_mem(int pid)
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
}

// Funciones para obtener o eliminar por pid de una lista (Revisar)
t_PCB remover_por_PID(t_list *lista, uint32_t numero)
{
    bool equivaleAlPID(t_PCB proceso)
    {
        return numero == proceso->PID;
    }
    return list_remove_by_condition(lista, equivaleAlPID);
}

bool encontrarProceso(t_list *lista, uint32_t numero)
{
    bool equivaleAlPID(t_PCB proceso)
    {
        return numero == proceso->PID;
    }
    return list_any_satisfy(lista, equivaleAlPID);
    // Buscar en cola de procesos y mandar a exit todos sus hilos
}

void send_pid_exit(int PID)
{
    void exit_process(void *ptr)
    {
        t_TCB tcb_aux = (t_TCB)ptr;
        //send_tid_exit(tcb_aux);
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