#include <../include/hello.h>
#include <../include/server.h>

int main(int argc, char *argv[])
{
    t_config *config = NULL;
    t_log *log = NULL;
    int i=0;

    config = levantar_config(getcwd(NULL, 0), "kernel");
    log = levantar_log(getcwd(NULL, 0), "kernel", config_get_string_value(config, "LOG_LEVEL"));

    log_info(log ,"Soy un log!");

    saludar("kernel");
    return 0;
}
