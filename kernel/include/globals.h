#ifndef H_GLOBALS_H_
#define H_GLOBALS_H_

#include <unistd.h>
#include <semaphore.h>
#include "../include/server.h"
#include "../include/protocolo.h"
#include "../include/hello.h"
#include "../include/operaciones_kernel.h"
#include "../include/tools.h"
#include "../include/procesos.h"
#include <readline/readline.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>

typedef enum
{
    FIFO,
    PRIORIDADES,
    CMN
} t_algoritmo;
extern t_algoritmo ALGORITMO_PLANIFICACION;

extern t_dictionary *dict;
extern t_config *config;
extern t_log *log;

// Semaforos y pthread
extern pthread_mutex_t mutex_cola_procesos;

extern pthread_mutex_t mutex_cola_new;
extern pthread_mutex_t mutex_cola_ready;
extern pthread_mutex_t mutex_cola_exit;
extern pthread_mutex_t mutex_cola_block;
extern sem_t sem_new;
extern sem_t sem_ready;
extern sem_t sem_exec;
extern sem_t sem_exit;
extern sem_t sem_block_return;

// queue - listas
extern t_list *process_list;
extern t_queue *new_queue;
extern t_list *ready_list;
extern t_list *blocked_queue;
extern t_list *exit_queue;

#endif