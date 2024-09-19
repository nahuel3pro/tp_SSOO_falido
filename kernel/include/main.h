#ifndef H_MAIN_H_
#define H_MAIN_H_

#include <unistd.h>
#include <../include/server.h>
#include <../include/protocolo.h>
#include <../include/hello.h>
#include "../include/operaciones_kernel.h"
#include "../include/tools.h"
#include "../include/procesos.h"
#include <readline/readline.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>

extern t_dictionary *dict;
extern t_config *config;
extern t_log *log;

#endif
