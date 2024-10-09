#include "../include/main.h"

t_queue *new_queue;
t_list *ready_list;

t_dictionary *dict;
t_config *config;
t_log *log;
char *algoritmo;
t_algoritmo ALGORITMO_PLANIFICACION = 0;

// Semaforos y pthread
pthread_mutex_t mutex_generador_pid;
pthread_mutex_t mutex_cola_ready;
pthread_mutex_t mutex_cola_listos_para_ready;
pthread_mutex_t mutex_cola_exit;
pthread_mutex_t mutex_cola_exec;
pthread_mutex_t mutex_cola_block;
pthread_mutex_t mutex_cola_block_io;
pthread_mutex_t mutex_cola_block_fs;
sem_t sem_multiprog;
sem_t sem_listos_ready;
sem_t sem_ready;
sem_t sem_exec;
sem_t sem_exit;
sem_t sem_block_return;
sem_t ongoing_fs_mem_op;
sem_t fin_f_open;

int main(int argc, char *argv[])
{
    // Definiendo variables para la ejecución del programa.
    new_queue = queue_create();
    ready_list = list_create();
    config = levantar_config(getcwd(NULL, 0), "kernel");
    log = levantar_log(getcwd(NULL, 0), "kernel", config_get_string_value(config, "LOG_LEVEL"));
    char *algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	asignar_algoritmo(algoritmo);
    log_info(log, "el algoritmo es: %d",ALGORITMO_PLANIFICACION);

    const char *path_to_psdc = argv[1];
    const int process_size = atoi(argv[2]);

    // Conectarse a memoria
/*     int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, socket_cliente, "Kernel/Memoria", KERNEL);

    // ------- HACER TODO ESTO EN UNA FUNCIÓN -------
    //  proceso 0
    t_PCB pcb = pcb_create(0, process_size);
    // hilo 0
    t_TCB tcb = tcb_create(0, HIGH, path_to_psdc);
    list_add(pcb->TIDs, tcb);

    // Serializar el proceso. Buffer con los datos del PCB--- Faltan datos.
    t_buffer *buffer = serializarProceso(pcb, path_to_psdc); // --- Faltan las listas.

    // empaquetar --------------- enviar pcb
    if (send_pcb(pcb, PROCESS_CREATION, buffer, socket_cliente) > 0)
    {
        log_info(log, "Proceso enviado");
        uint8_t res;
        if (recv(socket_cliente, &res, SIZEOF_UINT8, MSG_WAITALL) > 0 && res == (uint8_t)0)
        {
            log_info(log, "Proceso cargado exitosamente!");
        }
        else
        {
            log_error(log, "No hay memoria disponible, volviendo a la cola de ready...");
        }
    }
    else
    {
        log_error(log, "no se pudo mandar el proceso");
    }

    readline(">");
 */
    return 0;
}

// Conectarse a cpu dispatch
/* int socket_cliente = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
send_handshake(log, socket_cliente, "Kernel/CPU_dispatch", KERNEL);
enviar_paquete(paquete_instrucciones, socket_cliente); */
// // Conectarse a cpu interrupt
/* int socket_cliente = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
send_handshake(log, socket_cliente, "Kernel/CPU_interrupt", KERNEL); */

// Continuously call get_next_line until it returns NULL (EOF)
// while ((line = get_next_line(path_to_psdc)) != NULL)
// {
//     log_info(log, "%s", line);
//     readline("> ");
// }

void asignar_algoritmo(char *algoritmo)
{
    if (strcmp(algoritmo, "FIFO") == 0)
    {
        ALGORITMO_PLANIFICACION = FIFO;
    }
    else if (strcmp(algoritmo, "HRRN") == 0)
    {
        ALGORITMO_PLANIFICACION = CMN;
    }
    else
    {
        log_error(log, "El algoritmo no es valido");
    }
}