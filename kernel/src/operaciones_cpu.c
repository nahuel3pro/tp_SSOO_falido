#include "../include/operaciones_cpu.h"

void dispatch(t_TCB tcb, int fd)
{
    log_info(log, "Conectándome a CPU");
    //send_handshake(log, fd, "KERNEL/CPU", KERNEL);
    t_paquete *paquete_send = crear_paquete(DISPATCH);
    t_buffer *buffer_send = buffer_create(SIZEOF_UINT32 * 2);
    paquete_send->buffer = buffer_send;
    buffer_add_uint32(paquete_send->buffer, tcb->PID);
    buffer_add_uint32(paquete_send->buffer, tcb->TID);
    buffer_send->offset = 0;

    enviar_paquete(paquete_send, fd);
    log_info(log, "TCB mandado a CPU");
    eliminar_paquete(paquete_send);
}