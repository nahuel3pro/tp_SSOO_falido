#include <../include/operaciones_kernel.h>

t_list *recibir_instrucciones(char* file_name) {
    // Nombre del archivo
    const char *dir_actual = getcwd(NULL,0);
    t_list *lista = list_create();

    if(dir_actual == NULL){
        perror("Error al obtener el directorio actual");
        return NULL;
    }

    char filename[MAX_NOMBRE];
    snprintf(filename, sizeof(filename), "%s/test_psdc/%s", dir_actual, file_name);

    free(dir_actual);

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    char **lines = malloc(MAX_NOMBRE * sizeof(char *));
    if (lines == NULL) {
        perror("Error al asignar memoria para las líneas");
        fclose(file);
        return NULL;
    }

    char buffer[MAX_NOMBRE];
    int line_count = 0;

    // Leer el archivo línea por línea
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Asignar memoria para la línea y copiar el contenido del buffer
        lines[line_count] = malloc(strlen(buffer) + 1);
        if (lines[line_count] == NULL) {
            perror("Error al asignar memoria para una línea");
            // Liberar las líneas ya asignadas en caso de error
            for (int i = 0; i < line_count; i++) {
                free(lines[i]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }
        strcpy(lines[line_count], buffer);
        line_count++;
    }

    // Cerrar el archivo
    fclose(file);

    if (line_count < MAX_NOMBRE) {
        lines[line_count] = NULL;  // Añade un NULL al final para marcar el fin
    }

    for(int i = 0; lines[i] != NULL && i<MAX_OPERACIONES_LENGTH;i++){
        list_add(lista,lines[i]);
    }

    return lista;

}

// Función para liberar la memoria asignada por recibir_instrucciones
void liberar_instrucciones(t_list *lista) {
    // Verificar si la lista está vacía o es NULL
    if (lista == NULL || list_is_empty(lista)) {
        return;
    }
    
    // Limpiar y destruir los elementos de la lista utilizando free
    list_clean_and_destroy_elements(lista, (void*)free);
}

//Planificadores (a chequear)
void long_term_scheduler() {
    while (1) {
        // Verificar si hay procesos en la cola NEW para inicializarlos en Memoria
        if (!queue_is_empty(new_queue)) {
            t_PCB proceso = queue_pop(new_queue);
            // Aquí enviaríamos la solicitud a Memoria para inicializar el proceso
            log_event("Solicitud de creación de proceso enviada a Memoria");
        }
        // Continuar verificando finalización de procesos...
    }
}

// Planificador de corto plazo: selecciona el próximo hilo para ejecutar
void short_term_scheduler() {
    while (1) {
        if (!queue_is_empty(ready_queue)) {
            t_TCB thread = queue_pop(ready_queue);
            // Aquí se envía el hilo a la CPU para ser ejecutado
            log_event("Hilo enviado a CPU para ejecución");
        }
    }
}

//Syscalls
void process_create(char *filename, int size, int priority) {
    t_PCB new_process = malloc(sizeof(t_PCB));
    new_process->PID = rand();  // Generar un PID único
    new_process->TIDs = list_create();
    new_process->mutex = list_create();

    t_TCB main_thread = malloc(sizeof(t_TCB));
    main_thread->TID = 0;  // El hilo principal siempre tiene TID 0
    main_thread->PID = new_process->PID;
    main_thread->priority = priority;

    list_add(new_process->TIDs, main_thread);

    // Enviar el proceso a la cola NEW
    queue_push(new_queue, new_process);

    log_info(log,"Proceso creado y enviado a la cola NEW");
}

void process_exit(int pid) {
    // Buscamos el PCB del proceso a finalizar
    t_PCB process = find_process_by_pid(pid);  // Implementar una función que busque por PID
    
    if (process == NULL) {
        log_info(log,"Error: Proceso no encontrado");
        return;
    }

    // Liberamos los TCBs asociados al proceso
    list_iterate(process->TIDs, (void*) free);
    list_destroy(process->TIDs);

    // Informar a Memoria que el proceso ha terminado
    char *buffer[BUFFER_SIZE];
    sprintf(buffer, "Finalizar proceso con PID: %d", process->PID);
    int memory_socket = connect_to_memory("127.0.0.1", 8002);  // Conectar a Memoria
    send(memory_socket, buffer, strlen(buffer), 0);
    close(memory_socket);  // Cerramos la conexión

    // Liberamos el PCB
    free(process);

    log_info(log,"Proceso finalizado y notificado a Memoria");
}

void thread_create(int pid, int priority) {
    t_PCB process = find_process_by_pid(pid);
    
    if (process == NULL) {
        log_info("Error: Proceso no encontrado");
        return;
    }

    // Creamos un nuevo hilo
    t_TCB new_thread = malloc(sizeof(t_TCB));
    new_thread->TID = rand();  // Generar un TID único
    new_thread->PID = pid;
    new_thread->priority = priority;

    list_add(process->TIDs, new_thread);

    // Enviar el hilo a la cola READY
    queue_push(ready_queue, new_thread);
    
    log_info(log,"Hilo creado y enviado a la cola READY");
}

void thread_exit(int tid) {
    t_TCB thread = find_thread_by_tid(tid);
    
    if (thread == NULL) {
        log_info(log,"Error: Hilo no encontrado");
        return;
    }

    // Liberamos el TCB
    free(thread);

    log_info(log,"Hilo finalizado");
}

void thread_join(int tid_to_wait) {
    t_TCB waiting_thread = find_thread_by_tid(tid_to_wait);
    
    if (waiting_thread == NULL) {
        log_info(log,"Error: Hilo no encontrado para THREAD_JOIN");
        return;
    }

    // Bloqueamos el hilo hasta que el hilo 'tid_to_wait' finalice
    queue_push(blocked_queue, waiting_thread);

    log_info(log,"Hilo bloqueado en THREAD_JOIN");
}

void thread_cancel(int tid) {
    t_TCB thread_to_cancel = find_thread_by_tid(tid);
    
    if (thread_to_cancel == NULL) {
        log_info(log,"Error: Hilo no encontrado para THREAD_CANCEL");
        return;
    }

    // Liberamos el TCB
    free(thread_to_cancel);

    log_info(log,"Hilo cancelado");
}

void mutex_create(int pid, char *mutex_name) {
    t_PCB process = find_process_by_pid(pid);

    if (process == NULL) {
        log_info(log,"Error: Proceso no encontrado");
        return;
    }

    char *new_mutex = strdup(mutex_name);
    list_add(process->mutex, new_mutex);

    log_info(log,"Mutex creado");
}

void mutex_lock(int tid, char *mutex_name) {
    t_TCB thread = find_thread_by_tid(tid);
    t_PCB process = find_process_by_pid(thread->PID);

    if (thread == NULL) {
        log_info(log,"Error: Hilo no encontrado para MUTEX_LOCK");
        return;
    }

    // Verificar si el mutex existe y está disponible
    if (!mutex_is_available(mutex_name,process)) {
        queue_push(blocked_queue, thread);
        log_info(log,"Hilo bloqueado esperando mutex");
    } else {
        log_info(log,"Mutex adquirido");
    }
}

void mutex_unlock(int tid, char *mutex_name) {
    t_TCB thread = find_thread_by_tid(tid);

    if (thread == NULL) {
        log_info(log,"Error: Hilo no encontrado para MUTEX_UNLOCK");
        return;
    }

    // Liberar el mutex y desbloquear el siguiente hilo en espera
    unlock_mutex(mutex_name);
    t_TCB next_thread = queue_pop(blocked_queue);
    if (next_thread != NULL) {
        queue_push(ready_queue, next_thread);
        log_info(log,"Mutex liberado y siguiente hilo desbloqueado");
    }
}

void dump_memory(int pid, int tid) {
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "DUMP_MEMORY %d %d", pid, tid);
    
    int memory_socket = connect_to_memory("127.0.0.1", 8002);  // Conectar a Memoria
    send(memory_socket, buffer, strlen(buffer), 0);
    close(memory_socket);
    
    log_info(log,"Solicitud de DUMP_MEMORY enviada a Memoria");
}

//Fin syscalls

//Interrupción
void io(int tid, int duration_ms) {
    t_TCB *thread = find_thread_by_tid(tid);
    
    if (thread == NULL) {
        log_event("Error: Hilo no encontrado para IO");
        return;
    }

    // Bloquear el hilo por la duración de la operación de I/O
    queue_push(blocked_queue, thread);
    
    // Simular el tiempo de I/O (duración en milisegundos)
    usleep(duration_ms * 1000);

    // Mover el hilo a READY una vez que finalice la operación de I/O
    queue_push(ready_queue, thread);

    log_event("Operación de I/O finalizada, hilo movido a READY");
}

//Utilidades 

void* queue_get(t_queue* queue, int pos){
    t_queue *cola_fantasma;
    void* elemento;
    cola_fantasma = queue_create();
    cola_fantasma = queue;
    for (int i = 0; i<queue_size(cola_fantasma);i++){
        if (pos == i){
            elemento = queue_pop(cola_fantasma);
        }else
            queue_pop(cola_fantasma);
    } 
    queue_clean(cola_fantasma);
    queue_destroy(cola_fantasma);
    return elemento;
}

t_TCB find_thread_by_tid(int tid) {
    // Buscar en la cola de NEW
    for (int i = 0; i < queue_size(new_queue); i++) {
        t_PCB process = queue_get(new_queue, i);
        t_TCB thread = find_thread_in_pcb(process, tid);
        if (thread != NULL) {
            return thread;
        }
    }

    // Buscar en la cola de READY (FIFO o Prioridades)
    for (int i = 0; i < queue_size(ready_queue); i++) {
        t_TCB thread = queue_get(ready_queue, i);
        if (thread->TID == tid) {
            return thread;
        }
    }

    // Buscar en la cola de BLOCKED
    for (int i = 0; i < queue_size(blocked_queue); i++) {
        t_TCB thread = queue_get(blocked_queue, i);
        if (thread->TID == tid) {
            return thread;
        }
    }

    // Si no se encontró
    log_event("Error: Hilo no encontrado");
    return NULL;
}

t_TCB find_thread_in_pcb(t_PCB process, int tid) {
    for (int i = 0; i < list_size(process->TIDs); i++) {
        t_TCB thread = list_get(process->TIDs, i);
        if (thread->TID == tid) {
            return thread;
        }
    }
    return NULL;
}

t_PCB find_process_by_pid(int pid) {
    // Buscar en la cola NEW
    for (int i = 0; i < queue_size(new_queue); i++) {
        t_PCB process = queue_get(new_queue, i);
        if (process->PID == pid) {
            return process;
        }
    }

    // Buscar en la cola READY
    for (int i = 0; i < queue_size(ready_queue); i++) {
        t_PCB process = queue_get(ready_queue, i);
        if (process->PID == pid) {
            return process;
        }
    }

    // Buscar en la cola BLOCKED
    for (int i = 0; i < queue_size(blocked_queue); i++) {
        t_PCB process = queue_get(blocked_queue, i);
        if (process->PID == pid) {
            return process;
        }
    }

    // Buscar en la cola EXIT
    for (int i = 0; i < queue_size(exit_queue); i++) {
        t_PCB process = queue_get(exit_queue, i);
        if (process->PID == pid) {
            return process;
        }
    }

    // Si no se encuentra el proceso
    return NULL;
}

int mutex_is_available(char *mutex_name, t_PCB process) {
    for (int i = 0; i < list_size(process->mutex); i++) {
        char *mutex = list_get(process->mutex, i);
        if (strcmp(mutex, mutex_name) == 0) {
            // Aquí puedes comprobar si el mutex está en uso
            // Si está libre, retornamos 1 (disponible)
            return 1;
        }
    }
    // Mutex no encontrado o está en uso
    return 0;
}
// Inicializar la lista de mutexes activos
void init_mutexes() {
    active_mutexes = list_create();
}

// Función para liberar un mutex
void unlock_mutex(char *mutex_name) {
    // Verificar si el mutex está en la lista de mutexes activos
    for (int i = 0; i < list_size(active_mutexes); i++) {
        char *active_mutex = list_get(active_mutexes, i);
        if (strcmp(active_mutex, mutex_name) == 0) {
            // Si el mutex está encontrado, se libera
            list_remove(active_mutexes, i); // Remover de la lista de mutexes activos
            printf("Mutex '%s' liberado.\n", mutex_name);
            return;
        }
    }
    // Si el mutex no se encuentra
    printf("Error: Mutex '%s' no encontrado.\n", mutex_name);
}

//Fin Utilidades