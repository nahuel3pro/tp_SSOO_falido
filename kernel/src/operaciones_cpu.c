#include "../include/operaciones_cpu.h"

void dispatch(t_TCB tcb, int fd)
{
    log_info(log, "Conectándome a CPU");
    // send_handshake(log, fd, "KERNEL/CPU", KERNEL);
    t_paquete *paquete_send = crear_paquete(DISPATCH);
    t_buffer *buffer_send = buffer_create(SIZEOF_UINT32 * 2);
    paquete_send->buffer = buffer_send;
    buffer_add_uint32(paquete_send->buffer, tcb->PID);
    buffer_add_uint32(paquete_send->buffer, tcb->TID);
    buffer_send->offset = 0;

    enviar_paquete(paquete_send, fd);
    log_info(log, "TCB mandado a CPU");
    eliminar_paquete(paquete_send);
}

void atender_motivo(char *motivo, t_buffer *buffer_response)
{
    t_dictionary *dic = dict_protocol();
    int instruccion = (int)dictionary_get(dic, motivo);
    uint32_t tid;
    uint32_t pid;
    int priority;
    char *path_to_psdc;
    int srt_size;
    t_TCB tcb_aux;
    switch (instruccion)
    {
    case INSTRUCCION_PROCESS_CREATE:
        // esta syscall recibirá 3 parámetros de la CPU, el primero será el nombre del archivo de pseudocódigo que deberá ejecutar el proceso,
        // el segundo parámetro es el tamaño del proceso en Memoria y el tercer parámetro es la prioridad del hilo main (TID 0).
        // El Kernel creará un nuevo PCB y un TCB asociado con TID 0 y lo dejará en estado NEW.
        int srt_size;
        path_to_psdc = buffer_read_string(buffer_response, srt_size);
        int process_size = buffer_read_uint32(buffer_response);
        priority = buffer_read_uint32(buffer_response);

        // Separar la logica de crear proceso y hilo de esta funcion
        process_create(path_to_psdc, process_size, priority);
        // guardar en lista de procesos
        break;
    case INSTRUCCION_PROCESS_EXIT:
        // esta syscall finalizará el PCB correspondiente al TCB que ejecutó la instrucción, enviando todos sus TCBs asociados a la cola de EXIT.
        // Esta instrucción sólo será llamada por el TID 0 del proceso y le deberá indicar a la memoria la finalización de dicho proceso.
        tid = buffer_read_uint32(buffer_response);
        pid = buffer_read_uint32(buffer_response);
        if (tid == 0)
        {
            send_pid_exit(pid);
        }
        break;
    case INSTRUCCION_THREAD_CREATE:
        // esta syscall recibirá como parámetro de la CPU el nombre del archivo de pseudocódigo que deberá ejecutar el hilo a crear y su prioridad.
        // Al momento de crear el nuevo hilo, deberá generar el nuevo TCB con un TID autoincremental y poner al mismo en el estado READY.
        log_info(log, "INSTRUCCION_THREAD_CREATE");
        path_to_psdc = buffer_read_string(buffer_response, srt_size);
        priority = buffer_read_uint32(buffer_response);
        pid = buffer_read_uint32(buffer_response);

        thread_create(pid, priority, path_to_psdc);

        break;
    case INSTRUCCION_THREAD_JOIN:
        // esta syscall recibe como parámetro un TID, mueve el hilo que la invocó al estado BLOCK hasta que el TID pasado por parámetro finalice.
        // En caso de que el TID pasado por parámetro no exista o ya haya finalizado, esta syscall no hace nada y el hilo que la invocó continuará su ejecución.

        pid = buffer_read_uint32(buffer_response);
        tid = buffer_read_uint32(buffer_response);
        uint32_t tid_dependency = buffer_read_uint32(buffer_response);
        // Verificar que exista el TID y obtener el TCB
        if ((tcb_aux = get_thread(pid, tid_dependency)) != NULL)
        {
            tcb_aux->TID_wait = tid;
            // Si existe, Agregar TCB a lista BLOCK
            t_TCB thread_to_block = safe_tcb_remove(ready_list, &mutex_cola_ready);
            // list_add(blocked_queue, tcb)
            safe_tcb_add(blocked_queue, thread_to_block, &mutex_cola_block);
            log_info(log, "## (<%d>:<%d>) - Bloqueado por: <PTHREAD_JOIN>", pid, tid); //log obligatorio.
        }
        log_trace(log, "El hilo no existe");
        break;
    case INSTRUCCION_THREAD_CANCEL:
        //  esta syscall recibe como parámetro un TID con el objetivo de finalizarlo pasando al mismo al estado EXIT.
        //  Se deberá indicar a la Memoria la finalización de dicho hilo.
        //  En caso de que el TID pasado por parámetro no exista o ya haya finalizado, esta syscall no hace nada.
        //  Finalmente, el hilo que la invocó continuará su ejecución.
        uint32_t tid_to_end = buffer_read_uint32(buffer_response);
        pid = buffer_read_uint32(buffer_response);
        tcb_aux = get_thread(pid, tid_to_end);

        if(tcb_aux != NULL){
            pthread_mutex_lock(&mutex_cola_exec);
            pthread_mutex_lock(&mutex_cola_exit);
            list_remove_element(ready_list, tcb_aux);
            list_add(exit_queue, tcb_aux);
            pthread_mutex_unlock(&mutex_cola_exec);
            pthread_mutex_unlock(&mutex_cola_exit);
        }
        // Verificar que exista el TID y obtener el TCB
        // Si existe, Agregar TCB a lista EXIT y avisar a memoria
        // send_tid_exit(pid, tid_to_end);
        break;
    case INSTRUCCION_THREAD_EXIT:
        //  esta syscall finaliza al hilo que lo invocó, pasando el mismo al estado EXIT.
        //  Se deberá indicar a la Memoria la finalización de dicho hilo.
        pid = buffer_read_uint32(buffer_response);
        tid = buffer_read_uint32(buffer_response);
        log_info(log, "## (<%d>:<%d>) Finaliza el hilo", pid, tid);
        send_tid_exit();
        break;
    case INSTRUCCION_MUTEX_CREATE:
        // crea un nuevo mutex para el proceso sin asignar a ningún hilo.
        char *recurso = buffer_read_string(buffer_response, srt_size);
        pid = buffer_read_uint32(buffer_response);

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
        tid = buffer_read_uint32(buffer_response);
        pid = buffer_read_uint32(buffer_response);
        break;
    case -1:
        log_info(log, "Alguien no deseado quizo entrar");
        break;
    default:
        break;
    }

    dictionary_destroy(dic);
    // puede que esto lo tengan que hacer las funciones ---
    buffer_destroy(buffer_response);
    //---
}