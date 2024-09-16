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

t_buffer *buffer_create(void)
{
    // Creo y seteo las variables del buffer
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = 0;
    buffer->stream = NULL;

    return buffer;
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