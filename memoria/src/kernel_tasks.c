#include "../include/kernel_tasks.h"

void atenderKernel(void *void_args)
{
    t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
    int *socket_kernel_mem = args->fd;
    free(args);

    log_info(log, "## Kernel Conectado - FD del socket: <%d>", *socket_kernel_mem);

    t_paquete *paquete = malloc(sizeof(t_paquete));
    crear_buffer(paquete);

    // Primero recibimos el codigo de operacion
    recv(*socket_kernel_mem, &(paquete->op_code), SIZEOF_UINT8, 0);
    switch ((int)paquete->op_code)
    {
    case PROCESS_CREATION:
        uint8_t res = (uint8_t)SUCCESS;
        recv(*socket_kernel_mem, &(paquete->buffer->size), SIZEOF_UINT32, 0);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        recv(*socket_kernel_mem, paquete->buffer->stream, paquete->buffer->size, 0);
        // Des-serealizando ---> Crear función. y liberar la memoria a quien corresponda.
        uint32_t pid = buffer_read_uint32(paquete->buffer);
        uint32_t size = buffer_read_uint32(paquete->buffer);
        uint32_t str_size;
        char *path_read = buffer_read_string(paquete->buffer, &str_size);
        // Demorar tiempo y responder
        int wait_time = config_get_int_value(config, "RETARDO_RESPUESTA");
        sleep(wait_time); // Espera de un segundo
        log_info(log, "## Proceso <Creado> -  PID: <%d> - Tamaño: <%d>", pid, size);
        send(*socket_kernel_mem, &res, SIZEOF_UINT8, 0);
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
    // Se cierra la conexión con el kernel
    close(*socket_kernel_mem);
    free(socket_kernel_mem);
}
