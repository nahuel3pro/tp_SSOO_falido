#include "../include/operaciones_cpu.h"

void dispatch(t_TCB tcb)
{
    int fd = crear_conexion(config_get_string_value(config, "IP_CPU"), config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
    send_handshake(log, fd, "KERNEL/CPU", KERNEL);
    t_paquete *paquete = crear_paquete(DISPATCH);
    crear_buffer(paquete);
    buffer_add_uint32(paquete->buffer, tcb->PID);
    buffer_add_uint32(paquete->buffer, tcb->TID);
    enviar_paquete(paquete, fd);
    eliminar_paquete(paquete);
}