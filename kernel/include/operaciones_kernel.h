#ifndef H_OPERACIONES_KERNEL_H
#define H_OPERACIONES_KERNEL_H
#include <../include/main.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_OPERACIONES_LENGTH 20
#define MAX_NOMBRE 100

t_list *recibir_instrucciones(char* file_name);
void liberar_instrucciones(t_list* lista);

#endif //H_OPERACIONES_KERNEL_H
