#ifndef H_GLOBALS_H_
#define H_GLOBALS_H_

#include<semaphore.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include"../../utils/include/protocolo.h"
#include"../../utils/include/hello.h"
#include"../../utils/include/server.h"
#include<stdlib.h>

extern t_config *config;
extern t_log *log;

extern bool flag_interrupt;
extern pthread_mutex_t mutex_interrupt_signal;

#endif