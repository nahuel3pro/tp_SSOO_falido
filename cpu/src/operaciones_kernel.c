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
    // Memoria como sv
    server_dispatch_fd = iniciar_servidor(log, config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH"));

    while (1)
    {
        client_dispatch_fd = esperar_cliente(log, server_dispatch_fd);
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
    t_paquete * paquete = malloc(sizeof(t_paquete));

    crear_buffer(paquete);

    recv(client_dispatch_fd, &paquete->buffer->size, SIZEOF_UINT32, 0);

    paquete->buffer->stream = malloc(paquete->buffer->size);    

    recv(client_dispatch_fd, paquete->buffer->stream, paquete->buffer->size, 0);
    
    pid = buffer_read_uint32(paquete->buffer);
    tid = buffer_read_uint32(paquete->buffer);

    eliminar_paquete(paquete);

    int memoria_fd = crear_conexion(config_get_string_value(config, "IP_MEMORIA"), config_get_string_value(config, "PUERTO_MEMORIA"));
    send_handshake(log, memoria_fd, "CPU/MEMORIA", CPU);
    char * instruccion;
    t_register registro;

    fetch(memoria_fd, instruccion, &registro, pid, tid);
    decode_execute(instruccion, &registro, pid, tid);
}


void atender_interrupt(void)
{

}