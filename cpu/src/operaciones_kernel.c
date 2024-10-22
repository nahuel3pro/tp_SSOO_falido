#include "../include/operaciones_kernel.h"

void atenderKernel()
{
    pthread_t hilo_dispatch;
    pthread_t hilo_interrupt;
    pthread_create(&hilo_dispatch, NULL, (void *)atenter_dispatch, NULL);
    pthread_create(&hilo_interrupt, NULL, (void *)atender_interrupt, NULL);
    pthread_detach(hilo_dispatch);
    pthread_detach(hilo_interrupt);
}

void atenter_dispatch(void)
{
    uint8_t op_code;
    int server_dispatch_fd;
    int client_dispatch_fd;

    server_dispatch_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH"));
    client_dispatch_fd = esperar_cliente(log, server_dispatch_fd);

    while (1)
    {
        recv(client_dispatch_fd, &op_code, SIZEOF_UINT8, MSG_WAITALL);

        switch (op_code)
        {
        case DISPATCH:
            log_info(log, "Atendiendo instrucción dispatch...");
            dispatch(client_dispatch_fd);
            break;
        case -1:
            log_info(log, "Alguien no deseado quizo entrar");
            close(client_dispatch_fd);
            break;
        default:
            break;
        }
    }
}

void dispatch(int client_dispatch_fd)
{
    uint32_t pid;
    uint32_t tid;
    t_paquete *paquete = malloc(sizeof(t_paquete));

    crear_buffer(paquete);

    recv(client_dispatch_fd, &paquete->buffer->size, SIZEOF_UINT32, 0);

    paquete->buffer->stream = malloc(paquete->buffer->size);

    recv(client_dispatch_fd, paquete->buffer->stream, paquete->buffer->size, 0);

    pid = buffer_read_uint32(paquete->buffer);
    tid = buffer_read_uint32(paquete->buffer);

    eliminar_paquete(paquete);

    int memoria_fd = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, memoria_fd, "CPU/MEMORIA", CPU);
    char *instruccion;
    t_register registro;

    while (check_interrupt())
    {
        instruccion = fetch(memoria_fd, &registro, pid, tid);
        decode_execute(instruccion, &registro, pid, tid, client_dispatch_fd);
        update_context(memoria_fd, registro, pid, tid);
    }
}

bool check_interrupt()
{
    pthread_mutex_lock(&mutex_interrupt_signal);
    bool aux = flag_interrupt;
    pthread_mutex_unlock(&mutex_interrupt_signal);

    return aux;
}

void atender_interrupt(void)
{
    int server_dispatch_fd;
    int client_dispatch_fd;

    server_dispatch_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));
    client_dispatch_fd = esperar_cliente(log, server_dispatch_fd);
    log_info(log, "## Llega interrupción al puerto Interrupt"); // Log obligatorio
    pthread_mutex_lock(&mutex_interrupt_signal);
    flag_interrupt = false;
    pthread_mutex_unlock(&mutex_interrupt_signal);
}

void send_syscall(const char *instruction, int param_count, int client_dispatch_fd, ...)
{
    va_list args;
    va_start(args, instruction);
    t_buffer *buffer;
    char *arg1;
    char *arg2;
    char *arg3;

    int str_zise = strlen(instruction);
    switch (param_count)
    {
    case 1:
        buffer = buffer_create(SIZEOF_UINT32 + str_zise + 1);
        buffer_add_string(buffer, instruction);
        break;
    case 2:
        arg1 = va_arg(args, char *);
        buffer = buffer_create(SIZEOF_UINT32 * 2 + strlen(arg1) + str_zise + 2);
        buffer_add_string(buffer, instruction);
        buffer_add_string(buffer, arg1);
        break;
    case 3:
        arg1 = va_arg(args, char *);
        arg2 = va_arg(args, char *);
        buffer = buffer_create(SIZEOF_UINT32 * 3 + strlen(arg1) + strlen(arg2) + str_zise + 3);
        buffer_add_string(buffer, instruction);
        buffer_add_string(buffer, arg1);
        buffer_add_string(buffer, arg2);
        break;
    case 4:
        arg1 = va_arg(args, char *);
        arg2 = va_arg(args, char *);
        arg3 = va_arg(args, char *);
        buffer = buffer_create(SIZEOF_UINT32 * 4 + strlen(arg1) + strlen(arg2) + strlen(arg3) + str_zise + 4);
        buffer_add_string(buffer, instruction);
        buffer_add_string(buffer, arg1);
        buffer_add_string(buffer, arg2);
        buffer_add_string(buffer, arg3);
        break;
    default:
        log_error(log, "Te pasaste de parámetros");
        abort();
    }
    buffer->offset = 0;

    enviar_buffer(buffer, client_dispatch_fd);
    readline("> pausa para ver qué pasa en kernel");
}
