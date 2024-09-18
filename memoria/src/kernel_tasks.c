#include "../include/kernel_tasks.h"

void atenderKernel(void *void_args)
{
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    // t_log *logger = args->log;
    int *socket_kernel_mem = args->fd;
    free(args);

    log_info(log, "## Kernel Conectado - FD del socket: <%d>", *socket_kernel_mem);
    // uint8_t cod_op = recibir_operacion(*socket_kernel_mem);

    t_paquete *paquete = malloc(sizeof(t_paquete));
    crear_buffer(paquete);

    // Primero recibimos el codigo de operacion
    recv(*socket_kernel_mem, &(paquete->op_code), sizeof(uint8_t), 0);
    switch ((int)paquete->op_code)
    {
    case PROCESS_CREATION:
        uint8_t res = 0;
        recv(*socket_kernel_mem, &(paquete->buffer->size), sizeof(uint32_t), 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(*socket_kernel_mem, paquete->buffer->stream, paquete->buffer->size, 0);
        // des-serealizando ---> Crear función.
        uint32_t pid = buffer_read_uint32(paquete->buffer);
        uint32_t size = buffer_read_uint32(paquete->buffer);
        // demorar tiempo y responder
        log_info(log, "CREANDO PROCESO...");
        int wait_time = config_get_int_value(config, "RETARDO_RESPUESTA");
        sleep(wait_time/1000);
        log_info(log, "“## Proceso <Creado> -  PID: <%d> - Tamaño: <%d>”", pid, size);
        send(*socket_kernel_mem, &res, sizeof(uint8_t), 0);
        break;
    case PROCESS_KILL:
        log_info(log, "Matando el proceso");
        break;
    case THREAD_CREATION:
        log_info(log, "Creando thread...");
        break;
    case MEMORY_DUMP:
        log_info(log, "Dumpeando...");
        break;
    default:
        log_warning(log, "Operacion desconocida. No quieras meter la pata");
        break;
    }
    close(*socket_kernel_mem);
    free(socket_kernel_mem);
}
