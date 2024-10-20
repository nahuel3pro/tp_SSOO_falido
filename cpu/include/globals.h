#ifndef H_GLOBALS_H_
#define H_GLOBALS_H_

#include<commons/config.h>
#include<semaphore.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include"../../utils/include/protocolo.h"
#include"../../utils/include/hello.h"
#include"../../utils/include/server.h"
#include<stdlib.h>

extern t_config *config;
extern t_log *log;

extern bool flag_interrupt;
extern sem_t interrupt;

#endif