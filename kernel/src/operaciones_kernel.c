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
        int fd = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
        dispatch(tcb, fd);
        t_buffer *buffer_response = malloc(sizeof(t_buffer));
        buffer_recv(fd, buffer_response);
        int str_size;
        char* motivo = buffer_read_string(buffer_response, str_size);
        atender_motivo(motivo, buffer_response);
    }
}

void atender_motivo(char * motivo,t_buffer* buffer_response)
{
    t_dictionary* dic = dict_protocol();
    int instruccion = dictionary_get(dic, motivo);

    switch(instruccion)
        {
        case INSTRUCCION_PROCESS_CREATE:
        // esta syscall recibirá 3 parámetros de la CPU, el primero será el nombre del archivo de pseudocódigo que deberá ejecutar el proceso, 
        //el segundo parámetro es el tamaño del proceso en Memoria y el tercer parámetro es la prioridad del hilo main (TID 0). 
        //El Kernel creará un nuevo PCB y un TCB asociado con TID 0 y lo dejará en estado NEW.
            log_info(log, "INSTRUCCION_PROCESS_CREATE");
            int srt_size;
            char* path_to_psdc = buffer_read_string(buffer_response, srt_size );
            int process_size = buffer_read_uint32(buffer_response);
            int priority = buffer_read_uint32(buffer_response);

            //Separar la logica de crear proceso y hilo de esta funcion 
            process_create(path_to_psdc, process_size, priority);
            //guardar en lista de procesos            
            break;
        case INSTRUCCION_PROCESS_EXIT:
        // esta syscall finalizará el PCB correspondiente al TCB que ejecutó la instrucción, enviando todos sus TCBs asociados a la cola de EXIT. 
        // Esta instrucción sólo será llamada por el TID 0 del proceso y le deberá indicar a la memoria la finalización de dicho proceso.
            log_info(log, "INSTRUCCION_PROCESS_EXIT");
            uint32_t tid = buffer_read_uint32(buffer_response);
            uint32_t pid = buffer_read_uint32(buffer_response);
            if(tid == 0)
            {
                send_pid_exit(pid);
            }
            break;
        case INSTRUCCION_THREAD_CREATE:
        // esta syscall recibirá como parámetro de la CPU el nombre del archivo de pseudocódigo que deberá ejecutar el hilo a crear y su prioridad. 
        // Al momento de crear el nuevo hilo, deberá generar el nuevo TCB con un TID autoincremental y poner al mismo en el estado READY.
            log_info(log, "INSTRUCCION_THREAD_CREATE");
            int srt_size;
            char* path_to_psdc = buffer_read_string(buffer_response, srt_size);
            int priority = buffer_read_uint32(buffer_response);
            uint32_t pid = buffer_read_uint32(buffer_response);

            thread_create(pid , priority, path_to_psdc );

            break;
        case INSTRUCCION_THREAD_JOIN:
        // esta syscall recibe como parámetro un TID, mueve el hilo que la invocó al estado BLOCK hasta que el TID pasado por parámetro finalice. 
        // En caso de que el TID pasado por parámetro no exista o ya haya finalizado, esta syscall no hace nada y el hilo que la invocó continuará su ejecución.
            log_info(log, "INSTRUCCION_THREAD_JOIN");
            uint32_t tid = buffer_read_uint32(buffer_response);
            t_TCB
            //Verificar que exista el TID y obtener el TCB

            //Si existe, Agregar TCB a lista BLOCK
            //list_add(blocked_queue, tcb)

            //Como manejamos la sincronizacion entre este hilo y con el que hace join
            
            break;
        case INSTRUCCION_THREAD_CANCEL:
        //  esta syscall recibe como parámetro un TID con el objetivo de finalizarlo pasando al mismo al estado EXIT. 
        //  Se deberá indicar a la Memoria la finalización de dicho hilo. 
        //  En caso de que el TID pasado por parámetro no exista o ya haya finalizado, esta syscall no hace nada. 
        //  Finalmente, el hilo que la invocó continuará su ejecución.
            log_info(log, "INSTRUCCION_THREAD_CANCEL");
            uint32_t tid_to_end = buffer_read_uint32(buffer_response);
            uint32_t pid = buffer_read_uint32(buffer_response);

            //Verificar que exista el TID y obtener el TCB
            //Si existe, Agregar TCB a lista EXIT y avisar a memoria
            send_tid_exit(pid, tid_to_end);
            break;
        case INSTRUCCION_THREAD_EXIT:
        //  esta syscall finaliza al hilo que lo invocó, pasando el mismo al estado EXIT. 
        //  Se deberá indicar a la Memoria la finalización de dicho hilo.
            log_info(log, "INSTRUCCION_THREAD_EXIT");
            uint32_t tid = buffer_read_uint32(buffer_response);
            send_tid_exit(pid, tid);
            break;
        case INSTRUCCION_MUTEX_CREATE:
        // crea un nuevo mutex para el proceso sin asignar a ningún hilo.
            log_info(log, "INSTRUCCION_MUTEX_CREATE");
            int srt_size;
            char* recurso = buffer_read_string(buffer_response, srt_size);
            uint32_t pid = buffer_read_uint32(buffer_response);

            mutex_create(pid, recurso);
            break;
        case INSTRUCCION_MUTEX_LOCK:
        // se deberá verificar primero que exista el mutex solicitado y en caso de que exista y el mismo no se encuentre tomado se deberá asignar 
        // dicho mutex al hilo correspondiente. En caso de que el mutex se encuentre tomado, el hilo que realizó MUTEX_LOCK se bloqueará en la cola de 
        // bloqueados correspondiente a dicho mutex.

            log_info(log, "INSTRUCCION_MUTEX_LOCK");
            break;
        case INSTRUCCION_MUTEX_UNLOCK:
        // se deberá verificar primero que exista el mutex solicitado y esté tomado por el hilo que realizó la syscall. 
        // En caso de que corresponda, se deberá desbloquear al primer hilo de la cola de bloqueados de ese mutex y le asignará el mutex al hilo recién desbloqueado. 
        // Una vez hecho esto, se devuelve la ejecución al hilo que realizó la syscall MUTEX_UNLOCK. 
        // En caso de que el hilo que realiza la syscall no tenga asignado el mutex, no realizará ningún desbloqueo.
            log_info(log, "INSTRUCCION_MUTEX_UNLOCK");
            break;
        case INSTRUCCION_DUMP_MEMORY:
        // En este apartado solamente se tendrá la instrucción DUMP_MEMORY. 
        // Esta syscall le solicita a la memoria, junto al PID y TID que lo solicitó, que haga un Dump del proceso.
        // Esta syscall bloqueará al hilo que la invocó hasta que el módulo memoria confirme la finalización de la operación, 
        // en caso de error, el proceso se enviará a EXIT. Caso contrario, el hilo se desbloquea normalmente pasando a READY.            
            log_info(log, "INSTRUCCION_DUMP_MEMORY");
            uint32_t tid = buffer_read_uint32(buffer_response);
            uint32_t pid = buffer_read_uint32(buffer_response);
            break;    
        case -1:
            log_info(log, "Alguien no deseado quizo entrar");
            break;
        default:
            break;
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
    //Buscar en cola de procesos y mandar a exit todos sus hilos

}
