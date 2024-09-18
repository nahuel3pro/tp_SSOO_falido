#include "../include/protocolo.h"

bool send_handshake(t_log *logger, int fd_connection, const char *connection_name, int module)
{
    size_t bytes;
    int32_t result = 0;
    bytes = send(fd_connection, &module, sizeof(int), 0);
    bytes = recv(fd_connection, &result, sizeof(int32_t), MSG_WAITALL);

    if (result == 1)
    {
        log_info(logger, "Handshake OK de %s, bienvenido", connection_name);
        return true;
    }
    else
    {
        log_error(logger, "Handshake ERROR, no te conozco");
        close(fd_connection);
        return false;
    }
}

bool recv_handshake(t_log *log, int server_fd)
{
    size_t bytes;

    int32_t handshake;
    int32_t resultOk = 1;
    int32_t resultError = -1;

    bytes = recv(server_fd, &handshake, sizeof(int32_t), MSG_WAITALL);
    if (handshake == 1)
    {
        bytes = send(server_fd, &resultOk, sizeof(int32_t), 0);
        log_info(log, "Buenas tardes, caballero.");
    }
    else
    {
        bytes = send(server_fd, &resultError, sizeof(int32_t), 0);
        log_info(log, "Che, se quiso meter un desconocido.");
    }
    return true;
}

// Crea un buffer con el tamaño indicado
t_buffer *buffer_create(uint32_t size)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = size;
    buffer->offset = 0;
    buffer->stream = malloc(size);
    return buffer;
};

void buffer_destroy(t_buffer *buffer)
{
    free(buffer->stream);
    free(buffer);
}

t_dictionary *dict_protocol()
{

    t_dictionary *dict = dictionary_create();

    // Instrucciones básicas
    dictionary_put(dict, "SET", INSTRUCCION_SET);
    dictionary_put(dict, "SUM", INSTRUCCION_SUM);
    dictionary_put(dict, "SUB", INSTRUCCION_SUB);
    dictionary_put(dict, "LOG", INSTRUCCION_LOG);

    // Instrucciones de memoria
    dictionary_put(dict, "READ_MEM", INSTRUCCION_READ_MEM);
    dictionary_put(dict, "WRITE_MEM", INSTRUCCION_WRITE_MEM);
    dictionary_put(dict, "DUMP_MEMORY", INSTRUCCION_DUMP_MEMORY);

    // Control de flujo
    dictionary_put(dict, "JNZ", INSTRUCCION_JNZ);

    // Instrucciones de sincronización (Mutex)
    dictionary_put(dict, "MUTEX_CREATE", INSTRUCCION_MUTEX_CREATE);
    dictionary_put(dict, "MUTEX_LOCK", INSTRUCCION_MUTEX_LOCK);
    dictionary_put(dict, "MUTEX_UNLOCK", INSTRUCCION_MUTEX_UNLOCK);

    // Instrucciones de entrada/salida
    dictionary_put(dict, "IO", INSTRUCCION_IO);

    // Creación y finalización de procesos/hilos
    dictionary_put(dict, "PROCESS_CREATE", INSTRUCCION_PROCESS_CREATE);
    dictionary_put(dict, "PROCESS_EXIT", INSTRUCCION_PROCESS_EXIT);
    dictionary_put(dict, "THREAD_CREATE", INSTRUCCION_THREAD_CREATE);
    dictionary_put(dict, "THREAD_JOIN", INSTRUCCION_THREAD_JOIN);
    dictionary_put(dict, "THREAD_CANCEL", INSTRUCCION_THREAD_CANCEL);
    dictionary_put(dict, "THREAD_EXIT", INSTRUCCION_THREAD_EXIT);

    return dict;
}

t_buffer *serializarProceso(t_PCB pcb)
{
    t_buffer *buffer = buffer_create(sizeof(uint32_t) * 2);

    buffer_add_uint32(buffer, pcb->PID);
    buffer_add_uint32(buffer, pcb->size);

    return buffer;
}

// Agrega un stream al buffer en la posición actual y avanza el offset
void buffer_add(t_buffer *buffer, void *data, uint32_t size)
{
    memcpy(buffer->stream + buffer->offset, data, size);
    buffer->offset += size;
};

// Agrega un uint32_t al buffer
void buffer_add_uint32(t_buffer *buffer, uint32_t data)
{
    buffer_add(buffer, &data, sizeof(uint32_t));
}

// Agrega un uint8_t al buffer
void buffer_add_uint8(t_buffer *buffer, uint8_t data)
{
    buffer_add(buffer, &data, sizeof(uint8_t));
}

// Guarda size bytes del principio del buffer en la dirección data y avanza el offset
void buffer_read(t_buffer *buffer, void *data, uint32_t size)
{
    memcpy(data, buffer->stream + buffer->offset, size);
    buffer->offset += size;
};

// Lee un uint32_t del buffer y avanza el offset
uint32_t buffer_read_uint32(t_buffer *buffer)
{
    uint32_t data;
    buffer_read(buffer, &data, sizeof(uint32_t));
    return data;
}

// Lee un uint8_t del buffer y avanza el offset
uint8_t buffer_read_uint8(t_buffer *buffer)
{
    uint8_t data;
    buffer_read(buffer, &data, sizeof(uint8_t));
    return data;
}

// Lee un string y su longitud del buffer y avanza el offset
char *buffer_read_string(t_buffer *buffer, uint32_t *length)
{
    *length = buffer_read_uint32(buffer);
    char *string = malloc(*length);
    buffer_read(buffer, string, *length);
    return string;
}



int send_pcb(t_PCB pcb, op_code op_code, t_buffer *buffer, int socket_cliente)
{
    // empaquetar ---------------
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->op_code = (uint32_t)op_code;
    paquete->buffer = buffer;

    // OP CODE      // tamaño del stream   // stream
    void *a_enviar = serializar_paquete(paquete, paquete->buffer->size);
    // pruebas.

    // Por último enviamos
    int bytes = send(socket_cliente, a_enviar, sizeof(uint8_t) + sizeof(uint32_t) + buffer->size, 0);

    return bytes;
}

void *serializar_paquete(t_paquete *paquete, int bytes)
{
    void *magic = malloc(bytes);
    int desplazamiento = 0;

    memcpy(magic + desplazamiento, &(paquete->op_code), sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
    desplazamiento += paquete->buffer->size;

    return magic;
}

void enviar_mensaje(char *mensaje, int socket_cliente)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

    int bytes = paquete->buffer->size + 2 * sizeof(int);

    void *a_enviar = serializar_paquete(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    eliminar_paquete(paquete);
}

void crear_buffer(t_paquete *paquete)
{
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->offset = 0;
    paquete->buffer->stream = NULL;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio)
{
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

    memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
    memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

    paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete *paquete, int socket_cliente)
{
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serializar_paquete(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
}

void eliminar_paquete(t_paquete *paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

void liberar_conexion(int socket_cliente)
{
    close(socket_cliente);
}

t_paquete *crear_paquete(op_code codigo) // CREA BUFFER
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->op_code = codigo;
    crear_buffer(paquete); // Le inyecta un buffer
    return paquete;
}
