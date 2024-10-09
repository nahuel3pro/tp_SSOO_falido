#ifndef H_GLOBALS_H_
#define H_GLOBALS_H_

#include <unistd.h>
#include <semaphore.h>
#include <../include/server.h>
#include <../include/protocolo.h>
#include <../include/hello.h>
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
extern pthread_mutex_t mutex_generador_pid;
extern pthread_mutex_t mutex_cola_ready;
extern pthread_mutex_t mutex_cola_listos_para_ready;
extern pthread_mutex_t mutex_cola_exit;
extern pthread_mutex_t mutex_cola_exec;
extern pthread_mutex_t mutex_cola_block;
extern pthread_mutex_t mutex_cola_block_io;
extern pthread_mutex_t mutex_cola_block_fs;
extern sem_t sem_multiprog;
extern sem_t sem_listos_ready;
extern sem_t sem_ready;
extern sem_t sem_exec;
extern sem_t sem_exit;
extern sem_t sem_block_return;
extern sem_t ongoing_fs_mem_op;
extern sem_t fin_f_open;

// queue
extern t_queue *new_queue;
extern t_list *ready_list;
extern t_queue *blocked_queue;
extern t_queue *exit_queue;

#endif