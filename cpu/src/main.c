#include "../include/main.h"

t_config *config = NULL;
t_log *log = NULL;
bool flag_interrupt;
pthread_mutex_t mutex_interrupt_signal;

int main(int argc, char *argv[])
{
    inicializar_variables();
    atenderKernel();
    readline("> ");
    return EXIT_SUCCESS;
}

void inicializar_variables()
{
    config = levantar_config(getcwd(NULL, 0), "cpu");
    log = levantar_log(getcwd(NULL, 0), "cpu", config_get_string_value(config, "LOG_LEVEL"));

    flag_interrupt = true;
   pthread_mutex_init(&mutex_interrupt_signal, NULL);
}