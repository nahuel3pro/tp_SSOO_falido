#ifndef H_OPERACIONES_KERNEL_H
#define H_OPERACIONES_KERNEL_H
#include <../include/globals.h>
#include <commons/collections/queue.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_OPERACIONES_LENGTH 20
#define MAX_NOMBRE 100

/* extern t_queue *new_queue;
extern t_queue *ready_queue;
extern t_queue *blocked_queue;
extern t_queue *exit_queue; */


extern t_list *active_mutexes;
extern t_dictionary *dict;
extern t_config *config;
extern t_log *log;

t_list *recibir_instrucciones(char* file_name);
void liberar_instrucciones(t_list* lista);

//Planificadores (a chequear)
void short_term_scheduler();
void long_term_scheduler();
t_TCB safe_tcb_remove(t_list *list, pthread_mutex_t *mutex);
t_TCB elegir_tcb_segun_algoritmo();
void exec_tcb();

//Syscalls 
//Procesos 
void process_exit(int pid);
//Hilos
void thread_create(int pid, int priority, char *file_path);
void thread_exit(int tid);
void thread_join(int tid_to_wait);
void thread_cancel(int tid);
//Mutex
void mutex_create(int pid, char *mutex_name);
void mutex_lock(int tid, char *mutex_name);
void mutex_unlock(int tid, char *mutex_name);
//Memory
void dump_memory(int pid, int tid);
//io
void io(int tid, int duration_ms);
//Utilidades 
void* queue_get(t_queue *queue, int pos);
t_TCB find_thread_by_tid(int tid);
t_TCB find_thread_in_pcb(t_PCB process, int tid);
t_PCB find_process_by_pid(int pid);
int mutex_is_available(char *mutex_name, t_PCB process);
void init_mutexes();
void unlock_mutex(char *mutex_name);


#endif //H_OPERACIONES_KERNEL_H
