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

void exec_tcb()
{
    while (1)
    {
        sem_wait(&sem_ready);
        sem_wait(&sem_exec);
        t_TCB tcb = elegir_tcb_segun_algoritmo();
        // MANDAR A CPU EL TCB A EJECUTAR.
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
        return safe_tcb_remove(list_sorted(ready_list,_has_less_priority), &mutex_cola_ready);
        break;
    case CMN:
        log_info(log, "CMN");
        break;
    default:
        log_error(log, "No se reconocio el algoritmo de planifacion");
        exit(1);
    }
}

t_TCB safe_tcb_remove(t_list *list, pthread_mutex_t *mutex)
{
    t_TCB tcb;
    pthread_mutex_lock(mutex);
    tcb = list_remove(list, 0);
    pthread_mutex_unlock(mutex);
    return tcb;
}

void safe_tcb_add(t_list *list, t_TCB *tcb, pthread_mutex_t *mutex)
{
    pthread_mutex_lock(mutex);
    list_add(list, tcb);
    pthread_mutex_unlock(mutex);
}