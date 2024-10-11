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

void ready_tcb(void)
{
    while (1)
    {
        sem_wait(&sem_listos_ready); // cola new
        t_PCB pcb = safe_pcb_remove(new_queue, &mutex_cola_listos_para_ready);
        t_TCB aux = list_get(pcb->TIDs, 0);
        sem_post(&sem_ready);
        // mandarle a memoria PID, SIZE, Y RUTA DEL ARCHIVO.
        process_create(aux->file_path, pcb->size, aux->priority);
        // esperar confirmación de memoria
    }
}

void block_return_tcb()
{
    while (1)
    {
        sem_wait(&sem_block_return);
        t_TCB tcb = safe_tcb_remove(blocked_queue, &mutex_cola_block);
        //set_tcb_ready(tcb);
        sem_post(&sem_ready);
    }
}

void exec_tcb()
{
    while (1)
    {
       /*  sem_wait(&sem_ready);
        sem_wait(&sem_exec); */
        log_info(log, "Eligiendo TCB...");
        t_TCB tcb = elegir_tcb_segun_algoritmo();
        // MANDAR A CPU EL TCB A EJECUTAR.
        dispatch(tcb);
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
