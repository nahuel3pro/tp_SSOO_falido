#include "../include/main.h"

t_queue *new_queue;
t_list *ready_list;
t_list *exit_queue;
t_list *blocked_queue;
t_list *dependency_queue;


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

    config = levantar_config(getcwd(NULL, 0), "kernel");
    log = levantar_log(getcwd(NULL, 0), "kernel", config_get_string_value(config, "LOG_LEVEL"));
    char *algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    asignar_algoritmo(algoritmo);
    log_info(log, "el algoritmo es: %d", ALGORITMO_PLANIFICACION);

    inicializar_variables();

    /* const char *path_to_psdc = argv[1];
    const int process_size = atoi(argv[2]); */
    const char *path_to_psdc = "/home/utnso/Desktop/tp-2024-2c-La-Daneta/kernel/test_psdc/test2.dat";
    const int process_size = 58;

    // pruebas ----
    thread_create(555, 666, "Prueba 1234");

    readline("> ");
    // ----
    process_create(path_to_psdc, process_size, 0);

    planificar();

    // Conectarse a memoria
    readline("> ");

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

void inicializar_variables()
{
    exit_queue = list_create();
    new_queue = queue_create();
    ready_list = list_create();
    blocked_queue = list_create();
    dependency_queue = list_create();
    // fs_mem_op_count = 0;

    // Semaforos
    pthread_mutex_init(&mutex_generador_pid, NULL);
    pthread_mutex_init(&mutex_cola_ready, NULL);
    pthread_mutex_init(&mutex_cola_listos_para_ready, NULL);
    pthread_mutex_init(&mutex_cola_exit, NULL);
    pthread_mutex_init(&mutex_cola_exec, NULL);
    pthread_mutex_init(&mutex_cola_block, NULL);
    pthread_mutex_init(&mutex_cola_block_io, NULL);
    pthread_mutex_init(&mutex_cola_block_fs, NULL);

    sem_init(&sem_listos_ready, 0, 0);
    sem_init(&sem_ready, 0, 0);
    sem_init(&sem_exec, 0, 1);
    sem_init(&sem_exit, 0, 0);
    sem_init(&sem_block_return, 0, 0);
    sem_init(&ongoing_fs_mem_op, 0, 1);
    sem_init(&fin_f_open, 0, 0);
}

void planificar(){
    planificar_largo_plazo();
	log_info(log, "Se inició la planificacion de largo plazo");
	planificar_corto_plazo();
	log_info(log, "Se inició la planificacion de corto plazo");
}