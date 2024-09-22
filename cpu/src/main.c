#include "../include/main.h"

t_config *config = NULL;
t_log *log = NULL;

int main(int argc, char *argv[])
{
    t_list *instrucciones_kernel = list_create();
    // config = levantar_config(getcwd(NULL, 0), "cpu");
    // log = levantar_log(getcwd(NULL, 0), "cpu", config_get_string_value(config, "LOG_LEVEL"));
    int server_fd = 0;
    int client_fd = 0;

    config = levantar_config(getcwd(NULL, 0), "cpu");
    log = levantar_log(getcwd(NULL, 0), "cpu", config_get_string_value(config, "LOG_LEVEL"));

    int socket_cliente = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));

    // Supongamos que nos llega el pedido de kernel para ejecutar un proceso

    if (send_handshake(log, socket_cliente, "CPU/memoria", CPU))
    {
        /*  t_register *registro = malloc(sizeof(t_register));
         get_context(1, registro, socket_cliente); */


        char* instruccion = "SET AX 1";

        char** array = string_split(instruccion, " ");
        t_dictionary* dict = dict_protocol();
        int op_code = dictionary_get(dict, array[0]);
        switch (op_code)
        {
        case INSTRUCCION_SET:
            log_info(log, "Se recibio la instruccion SET");
            log_debug(log, "Se recibio la instruccion SET");
            log_trace(log, "Se recibio la instruccion SET");
            log_error(log, "Se recibio la instruccion SET");
            asignar_registro_beta(array[1], atoi(array[2]) );
            
            array[1];
            break;
        
        default:
            break;
        }


        readline(">");
    }

    // readline("> ");
    return EXIT_SUCCESS;
}
