#ifndef UTILS_HELLO_H_
#define UTILS_HELLO_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>

/**
 * @brief Imprime un saludo por consola
 * @param quien Módulo desde donde se llama a la función
 * @return No devuelve nada
 */
void saludar(char *quien);
char *concatenate_route(char *path, char *module, char *file_format);
t_config *levantar_config(char *path, char *module);
t_log *levantar_log(char *path, char *module, char *level);

#endif
