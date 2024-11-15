#include "../include/hello.h"

void saludar(char *quien)
{
    printf("Hola desde %s!!\n", quien);
}

char *concatenate_route(char *path, char *module, char *file_format)
{
    char *dir_bar = "/";

    size_t len1 = strlen(path);
    size_t len2 = strlen(dir_bar);
    size_t len3 = strlen(module);
    size_t len4 = strlen(file_format);
    char *file_dirr = (char *)malloc(len1 + len2 + len3 + len4 + 1);

    strcpy(file_dirr, path);
    strcat(file_dirr, dir_bar);
    strcat(file_dirr, module);
    strcat(file_dirr, file_format);

    return file_dirr;
}

t_config *levantar_config(char *path, char *module)
{
    t_config *nuevo_config;
    char *file_format = ".config";
    char *file_dirr = concatenate_route(path, module, file_format);

    if ((nuevo_config = config_create(file_dirr)) == NULL)
    {
        printf("Fallo al inicializar la config");
        exit(1);
    }
    printf("Config de %s cargada!\n", module);
    free(file_dirr);
    return nuevo_config;
}

t_log *levantar_log(char *path, char *module, char *log_level)
{
    t_log *nuevo_logger;
    char *file_format = ".log";
    char *file_dirr = concatenate_route(path, module, file_format);

    if ((nuevo_logger = log_create(file_dirr, module, 1, log_level_from_string(log_level))) == NULL)
    {
        printf("Fallo al inicializar el logger!");
        exit(1);
    }
    printf("Logger de %s cargado!\n", module);
    free(file_dirr);
    return nuevo_logger;
}

/* void terminar_programa(int conexion, t_log *logger, t_config *config)
{
    if (logger != NULL)
    {
        log_destroy(logger);
    }
    if (config != NULL)
    {
        config_destroy(config);
    }
    if (conexion)
    {
        close(conexion);
    }
} */
