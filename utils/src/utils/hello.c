#include "../include/hello.h"

void saludar(char *quien)
{
    printf("Hola desde %s!!\n", quien);
}

t_config *levantar_config(char *path, char *module)
{
    t_config *nuevo_config;
    char *file_name = ".config";

    size_t len1 = strlen(path);
    size_t len2 = strlen(module);
    size_t len3 = strlen(file_name);
    char *file_dirr = (char *)malloc(len1 + len2 + len3 + 1);

    strcpy(file_dirr, path);
    strcat(file_dirr, module);
    strcat(file_dirr, file_name);

    if ((nuevo_config = config_create(file_dirr)) == NULL)
    {
        printf("Fallo al inicializar la config");
        exit(1);
    }
    free(file_dirr);
    return nuevo_config;
}
