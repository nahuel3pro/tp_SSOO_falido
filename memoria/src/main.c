#include "../include/main.h"

int main(int argc, char *argv[])
{
    t_config *config;
    t_log *log;

    config = levantar_config(getcwd(NULL, 0), "memoria");
    log = levantar_log(getcwd(NULL, 0), "memoria", config_get_int_value(config, "LOG_LEVEL"));

    readline("> ");

    log_destroy(log);
    config_destroy(config);
    saludar("memoria");
    return 0;
}
