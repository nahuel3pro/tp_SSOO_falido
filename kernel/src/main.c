#include "../include/main.h"

t_list *process_list;
t_queue *new_queue;
t_list *ready_list;
t_list *exit_queue;
t_list *blocked_queue;


t_dictionary *dict;
t_config *config;
t_log *log;
char *algoritmo;
t_algoritmo ALGORITMO_PLANIFICACION = 0;

// Semaforos y pthread
pthread_mutex_t mutex_cola_procesos;

pthread_mutex_t mutex_generador_pid;
pthread_mutex_t mutex_cola_ready;
pthread_mutex_t mutex_cola_listos_para_ready;
pthread_mutex_t mutex_cola_exit;
pthread_mutex_t mutex_cola_exec;
pthread_mutex_t mutex_cola_block;
pthread_mutex_t mutex_cola_block_io;
pthread_mutex_t mutex_cola_block_fs;
sem_t sem_multiprog;
sem_t sem_new;
sem_t sem_ready;
sem_t sem_exec;
sem_t sem_exit;
sem_t sem_block_return;
sem_t ongoing_fs_mem_op;

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
    /* process_create(path_to_psdc, process_size, 0);
    thread_create(1804289383, 666, "/home/utnso/Desktop/tp-2024-2c-La-Daneta/kernel/test_psdc/test.dat");


    readline("> "); */
    // ----
    process_create(path_to_psdc, process_size, 0);

    planificar();

    // Conectarse a memoria
    readline("> ");


    return SUCCESS;
}

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
        abort();
    }
}

void inicializar_variables()
{
    process_list = list_create(); 
    exit_queue = list_create();
    new_queue = queue_create();
    ready_list = list_create();
    blocked_queue = list_create();
    // fs_mem_op_count = 0;

    // Semaforos
    pthread_mutex_init(&mutex_cola_procesos, NULL);

    pthread_mutex_init(&mutex_generador_pid, NULL);
    pthread_mutex_init(&mutex_cola_ready, NULL);
    pthread_mutex_init(&mutex_cola_listos_para_ready, NULL);
    pthread_mutex_init(&mutex_cola_exit, NULL);
    pthread_mutex_init(&mutex_cola_exec, NULL);
    pthread_mutex_init(&mutex_cola_block, NULL);
    pthread_mutex_init(&mutex_cola_block_io, NULL);
    pthread_mutex_init(&mutex_cola_block_fs, NULL);

    sem_init(&sem_new, 0, 0);
    sem_init(&sem_ready, 0, 0);
    sem_init(&sem_exec, 0, 1);
    sem_init(&sem_exit, 0, 0);
    sem_init(&sem_block_return, 0, 0);
    sem_init(&ongoing_fs_mem_op, 0, 1);
}

void planificar(){
    planificar_largo_plazo();
	log_trace(log, "Se inició la planificacion de largo plazo");
	planificar_corto_plazo();
	log_trace(log, "Se inició la planificacion de corto plazo");
}