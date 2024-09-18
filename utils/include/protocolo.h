#ifndef UTILS_PROTOCOLO_H_
#define UTILS_PROTOCOLO_H_

#include <commons/collections/dictionary.h>
#include <stdlib.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "procesos.h"

typedef enum
{
    // Instrucciones Básicas
    INSTRUCCION_SET,
    INSTRUCCION_SUM,
    INSTRUCCION_SUB,
    INSTRUCCION_LOG,

    // Instrucciones de Memoria
    INSTRUCCION_READ_MEM,
    INSTRUCCION_WRITE_MEM,
    INSTRUCCION_DUMP_MEMORY,

    // Control de Flujo
    INSTRUCCION_JNZ,

    // Sincronización (Mutex)
    INSTRUCCION_MUTEX_CREATE,
    INSTRUCCION_MUTEX_LOCK,
    INSTRUCCION_MUTEX_UNLOCK,

    // Entrada/Salida
    INSTRUCCION_IO,

    // Creación y Finalización de Procesos/Hilos
    INSTRUCCION_PROCESS_CREATE,
    INSTRUCCION_PROCESS_EXIT,
    INSTRUCCION_THREAD_CREATE,
    INSTRUCCION_THREAD_JOIN,
    INSTRUCCION_THREAD_CANCEL,
    INSTRUCCION_THREAD_EXIT

} op_instruct;
typedef enum
{
    KERNEL,
    CPU,
    MEMORIA,
    FILESYSTEM
} op_module;

typedef enum
{
    PROCESS_CREATION,
    PROCESS_KILL,
    THREAD_CREATION,
    MEMORY_DUMP
} op_code;

typedef struct
{
    uint32_t size;   // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void *stream;    // Payload
} t_buffer;

typedef struct
{
    uint8_t op_code;
    t_buffer *buffer;
} t_paquete;

/**
 * @brief Intenta entrar en contacto con un servidor.
 * @param logger loger.
 * @param fd Socket de conexión del cliente.
 * @param connection_name Modulos que establecen su conexión ej: Kernel/Memoria.
 * @param module constante enum del propio cliente para que el servidor identifique.
 * @return false si no se aceptó, true si se aceptó la conexión.
 */
bool send_handshake(t_log *logger, int fd, const char *connection_name, int module);
bool recv_handshake(t_log *logger, int server_fd);
t_buffer *buffer_create(uint32_t size);
void buffer_destroy(t_buffer *buffer);

t_dictionary *dict_protocol();

void enviar_mensaje(char *mensaje, int socket_cliente);
t_paquete *crear_paquete(op_code codigo);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete *paquete);

t_buffer *serializarProceso(t_PCB pcb);
void *serializar_paquete(t_paquete *paquete, int bytes);
void buffer_add(t_buffer *buffer, void *data, uint32_t size);
void buffer_add_uint32(t_buffer *buffer, uint32_t data);
void buffer_read(t_buffer *buffer, void *data, uint32_t size);
uint32_t buffer_read_uint32(t_buffer *buffer);
uint8_t buffer_read_uint8(t_buffer *buffer);
char *buffer_read_string(t_buffer *buffer, uint32_t *length);

int send_pcb(t_PCB pcb, op_code op_code, t_buffer *buffer, int socket_cliente);
void crear_buffer(t_paquete *paquete);

#endif