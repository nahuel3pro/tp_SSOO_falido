#ifndef UTILS_HELLO_H_
#define UTILS_HELLO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>

#define SIZEOF_UINT64 sizeof(uint64_t)
#define SIZEOF_UINT32 sizeof(uint32_t)
#define SIZEOF_UINT16 sizeof(uint16_t)
#define SIZEOF_UINT8 sizeof(uint8_t)

char *concatenate_route(char *path, char *module, char *file_format);
t_config *levantar_config(char *path, char *module);
t_log *levantar_log(char *path, char *module, char *level);
void terminar_programa(int conexion, t_log *logger, t_config *config);


#endif
