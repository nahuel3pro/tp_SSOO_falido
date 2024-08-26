#include "../include/main.h"

int main(int argc, char *argv[])
{
    t_config *config = NULL;
    t_log *log = NULL;
    int server_fd = 0;
    int cliente_fd = 0;
    
    config = levantar_config(getcwd(NULL, 0), "memoria");
    log = levantar_log(getcwd(NULL, 0), "memoria", config_get_string_value(config, "LOG_LEVEL"));

    server_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA"));
    cliente_fd = esperar_cliente(log, server_fd);

    //readline("> ");

    return EXIT_SUCCESS;
}
