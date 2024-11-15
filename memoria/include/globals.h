#ifndef H_GLOBALS_H_
#define H_GLOBALS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include "../include/procesos.h"
#include <../include/server.h>
#include <../include/hello.h>
#include <../include/protocolo.h>

extern t_config *config;
extern t_log *log;

extern t_list *process_list;
extern pthread_mutex_t mutex_process_list;
#endif